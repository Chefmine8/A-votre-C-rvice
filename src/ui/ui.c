#include "struct.h"


/* Crée un dossier si il n'existe pas déjà. */
int create_directory_if_not_exists(const char *path)
{
    struct stat st = {0};

    if (stat(path, &st) == -1) {
        if (mkdir(path, 0700) != 0) {
            perror("mkdir");
            return 0;
        }
    }
    return 1;
}

/* Permet d'effacer le dossier au moment de quitter. */
int remove_directory(const char *path) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf) {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = remove_directory(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}

/* Adapte l'image à la taille de la fenetre en concervant le bon ratio. */
void get_scaled_size(int orig_width, int orig_height, int max_width, int max_height, int *new_width, int *new_height) {
    double ratio = (double)orig_width / (double)orig_height;

    if (orig_width > orig_height) {
        *new_width = max_width;
        *new_height = (int)(max_width / ratio);
        if (*new_height > max_height) {
            *new_height = max_height;
            *new_width = (int)(max_height * ratio);
        }
    } else {
        *new_height = max_height;
        *new_width = (int)(max_height * ratio);
        if (*new_width > max_width) {
            *new_width = max_width;
            *new_height = (int)(max_width / ratio);
        }
    }
}

/* Sauvegarde img.txt. */
void save_file(GtkWidget *fenetre, gpointer user_data)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);
    GtkTextIter start, end;
    gchar *texte;

    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    texte = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    g_file_set_contents("img", texte, -1, NULL);

    g_free(texte);
}


/* Fonction qui transmet au reseau de neurone. */
void nn_transf(GtkButton *button, gpointer user_data) {
    int nb_arr = 3;
    int arr[3] = {784,784,26};
    struct neural_network *nn = create_neural_network(nb_arr,arr);
    create_grid("img",nn);
    create_word_list("word_list",nn);
    free_neural_network(nn);

    GtkWidget *fenetre;
    GtkWidget *scroll;
    GtkWidget *textview;
    GtkTextBuffer *buffer;

    fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fenetre), "Éditeur de texte");
    gtk_window_set_default_size(GTK_WINDOW(fenetre), 600, 400);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(fenetre), scroll);

    textview = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scroll), textview);

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

    gchar *contenu = NULL;
    gsize longueur;

    if (g_file_get_contents("img", &contenu, &longueur, NULL)) {
        gtk_text_buffer_set_text(buffer, contenu, -1);
        g_free(contenu);
    }
    g_signal_connect(fenetre, "destroy", G_CALLBACK(save_file), buffer);
    gtk_widget_show_all(fenetre);
}

/* Fonction qui gére le pré processing */
void rotate_image(GtkWidget *window, GtkEntry *entry, double manual_angle)
{
    const gchar *text_tmp = gtk_entry_get_text(entry);
    if (!text_tmp || *text_tmp == '\0') {
        g_print("No file path provided.\n");
        return;
    }
    gchar *text = g_strdup(text_tmp);
    g_object_set_data_full(G_OBJECT(window), "current-filename", g_strdup(text), g_free);

    /* Blank screen */
    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(iter->data);
    g_list_free(children);


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    Image *img = load_image(text);
    Image *original = load_image(text);

    grayscale_image(img);

    Image *tmp = sauvola(img, 12, 128, 0.07);
    free_image(img);
    img = tmp;

    if (manual_angle != 0.0)
    {
        Image *rotated = manual_rotate_image(img, -manual_angle);
        free_image(img);
        img = rotated;
    }
    else
    {
        double angle = get_auto_rotation_angle(img);
        Image *rotated = manual_rotate_image(img, -angle);
        free_image(img);
        img = rotated;
    }

    Shape **shapes = get_all_shape(img);

    remove_small_shape(img, shapes, 8);
    remove_outliers_shape(img, shapes, 25, 75, 2.5, 3);
    remove_aspect_ration(img, shapes, 0.1, 5);

    Image* before = copy_image(img);

    for (int y = 0; y < before->height; y++)
    {
        for (int x = 0; x < before->width; x++)
        {
            get_pixel(before, x, y)->isInShape = 0;
            get_pixel(before, x, y)->shape_ptr = NULL;
        }
    }
    Image *circle = circle_image(img, shapes, 0.25);
    int theta_range, rho_max;
    int **hs = hough_space(circle, &theta_range, &rho_max);
    hough_space_filter(hs, theta_range, rho_max, 0.495);

    filter_line(hs, theta_range, rho_max, 15 ,20);

    int **h_lines = horizontal_lines(hs, theta_range, rho_max, 5);
    int **v_lines = vertical_lines(hs, theta_range, rho_max, 5);

    filter_gaps(h_lines, theta_range, rho_max);
    filter_gaps(v_lines, theta_range, rho_max);

    int x_start, y_start, x_end, y_end;
    get_bounding_box(v_lines, h_lines, theta_range, rho_max, &x_start, &x_end, &y_start, &y_end);

    free_hough(h_lines, theta_range);
    free_hough(v_lines, theta_range);
    free_hough(hs, theta_range);

    clean_shapes(shapes);
    free_image(circle);

    double mean_shape_width = 0.0, mean_shape_height = 0.0;
    int shape_count = 0;
    for (int j = 0; shapes[j] != NULL; j++)
    {
        shape_count++;
        mean_shape_width += shape_width(shapes[j]);
        mean_shape_height += shape_height(shapes[j]);
    }
    mean_shape_width /= shape_count;
    mean_shape_height /= shape_count;


    int offset_x = ceil(mean_shape_width *1.6);
    int offset_y = ceil(mean_shape_height *1.2);
    x_start = x_start - offset_x < 0 ? 0 : x_start - offset_x;
    y_start = y_start - offset_y < 0 ? 0 : y_start - offset_y;
    x_end = x_end + offset_x >= img->width ? img->width - 1 : x_end + offset_x;
    y_end = y_end + offset_y >= img->height ? img->height - 1 : y_end + offset_y;

    if (x_start < img->width * 0.1 && x_end > img->width * 0.90)
    {
        x_start = offset_x;
        x_end = img->width - offset_x;
    }

    draw_line(img, x_start - 1 < 0 ? 0 : x_start - 1, y_start - 1 < 0 ? 0 : y_start - 1, x_end + 1 >= img->width ? img->width - 1 : x_end + 1, y_start - 1 < 0 ? 0 : y_start - 1, 0, 255, 0);
    draw_line(img, x_start - 1 < 0 ? 0 : x_start - 1, y_end + 1 >= img->height ? img->height - 1 : y_end + 1, x_end + 1 >= img->width ? img->width - 1 : x_end + 1, y_end + 1 >= img->height ? img->height - 1 : y_end + 1, 0, 255, 0);
    draw_line(img, x_start - 1 < 0 ? 0 : x_start - 1, y_start - 1 < 0 ? 0 : y_start - 1, x_start - 1 < 0 ? 0 : x_start - 1, y_end + 1 >= img->height ? img->height - 1 : y_end + 1, 0, 255, 0);
    draw_line(img, x_end + 1 >= img->width ? img->width - 1 : x_end + 1, y_start - 1 < 0 ? 0 : y_start - 1, x_end + 1 >= img->width ? img->width - 1 : x_end + 1, y_end + 1 >= img->height ? img->height - 1 : y_end + 1, 0, 255, 0);

    Image *sub_img = extract_sub_image(before, x_start, y_start, x_end, y_end);

    Shape *sub_shape = create_shape();
    for (int y = y_start - 30 < 0 ? 0 : y_start - 30; y <= (y_end + 30 >= before->height ? before->height -1 : y_end + 30); y++)
    {
        for (int x = x_start - 30 < 0 ? 0 : x_start - 30; x <= (x_end + 30 >= before->width ? before->width -1 : x_end + 30); x++)
        {
            shape_add_pixel(sub_shape, get_pixel(before, x, y));
        }
    }
    image_remove_shape(before, sub_shape);
    free_shape(sub_shape);

    Shape ** before_shapes = get_all_shape(before);
    remove_small_shape(before, before_shapes, 20);
    remove_aspect_ration(before, before_shapes, 0.1, 6);


    filter_by_density_v(before,before_shapes, 1);
    clean_shapes(before_shapes);

    Shape ***words = get_all_world(before_shapes);

    int valid_word_count = 0;
    for (int w = 0; words[w] != NULL; w++)
    {
        Shape **word = words[w];
        int letter_count = 0;
        for (int l = 0; word[l] != NULL; l++)
        {
            letter_count++;
        }
        if (letter_count <= 15)
        {
            words[valid_word_count] = words[w];
            valid_word_count++;
        }
        else
        {
            free(word);
        }
    }
    words[valid_word_count] = NULL;

    remove_directory("./output/");
    create_directory_if_not_exists("./output");
    create_directory_if_not_exists("./output/word_list");
    for (int w = 0; words[w] != NULL; w++)
    {
        Shape **word = words[w];
        for (int l = 0; word[l] != NULL; l++)
        {
            Shape *letter = word[l];
            char letter_filename[256];
            snprintf(letter_filename, sizeof(letter_filename), "./output/word_list/word_%d_letter_%d.png", w + 1, l + 1);
            int start_x = letter->min_x;
            int start_y = letter->min_y;
            int end_x = letter->max_x;
            int end_y = letter->max_y;
            Image *letter_img = extract_sub_image(before, start_x, start_y, end_x, end_y);
            SDL_Surface *letter_surf = image_to_sdl_surface(letter_img);
            export_image(letter_surf, letter_filename);
            SDL_FreeSurface(letter_surf);
            free_image(letter_img);
        }
    }

    for (int w = 0; words[w] != NULL; w++)
    {
        free(words[w]);
    }
    free(words);
    free_image(before);

    for (int j = 0; before_shapes[j] != NULL; j++)
    {
        free_shape(before_shapes[j]);
    }
    free(before_shapes);

    if (sub_img->height > 500)
    {
        int height_objective = (int)(sub_img->height * 0.8);
        float scale_x = (float)height_objective / (float)(sub_img->height);
        Image *scale = manual_image_scaling(sub_img, scale_x, scale_x);
        free_image(sub_img);
        sub_img = scale;
    }
    else
    {
        int height_objective = (int)(sub_img->height * 1.3);
        float scale_x = (float)height_objective / (float)(sub_img->height);
        Image *scale = manual_image_scaling(sub_img, scale_x, scale_x);
        free_image(sub_img);
        sub_img = scale;
    }

    Shape **sub_shapes = get_all_shape(sub_img);

    remove_small_shape(sub_img,sub_shapes , 15);
    remove_outliers_shape(sub_img,sub_shapes , 25,75,2.5,3);
    remove_aspect_ration(sub_img,sub_shapes , 0.1, 2);

    clean_shapes(sub_shapes);

    merge_shapes(sub_shapes, 3);

    filter_by_density(sub_img,sub_shapes, 5);
    clean_shapes(sub_shapes);

    int rows, cols;
    detect_grid_size(sub_shapes, &rows, &cols);

    int min = rows < cols ? rows : cols;
    filter_by_density(sub_img, sub_shapes, (int)(min * 0.9));
    clean_shapes(sub_shapes);

    detect_grid_size(sub_shapes, &rows, &cols);


    create_directory_if_not_exists("./output");
    char filename[256];
    snprintf(filename, sizeof(filename), "./output/img_%d_%d",rows, cols);
    create_directory_if_not_exists(filename);
    Image *** cell_images = get_grid_cells(sub_img,sub_shapes, rows, cols, "./output/map");
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (cell_images[r][c] == NULL)
                continue;
            char cell_filename[256];
            snprintf(cell_filename, sizeof(cell_filename), "./output/img_%d_%d/cell_%d_%d.png",rows, cols, c + 1, r + 1);
            SDL_Surface *cell_surf = image_to_sdl_surface(cell_images[r][c]);
            export_image(cell_surf, cell_filename);
            SDL_FreeSurface(cell_surf);
            free_image(cell_images[r][c]);
        }
        free(cell_images[r]);
    }
    free(cell_images);

    for (int j = 0; sub_shapes[j] != NULL; j++)
    {
        free_shape(sub_shapes[j]);
    }
    free(sub_shapes);

    free_image(sub_img);

    for (int j = 0; shapes[j] != NULL; j++)
    {
        free_shape(shapes[j]);
    }
    free(shapes);

    export_image(image_to_sdl_surface(img), "rotate_tmp.png");


    /* Affichage */
    int display_width = 400;
    int display_height = 400;

    GdkPixbuf* pixbuf_original = gdk_pixbuf_new_from_file(text, NULL);
    int new_width, new_height;
    get_scaled_size(gdk_pixbuf_get_width(pixbuf_original), gdk_pixbuf_get_height(pixbuf_original),
                    400, 400, &new_width, &new_height);
    GdkPixbuf* pixbuf_scaled_original = gdk_pixbuf_scale_simple(
        pixbuf_original, new_width, new_height, GDK_INTERP_BILINEAR);
    GtkWidget* image_original = gtk_image_new_from_pixbuf(pixbuf_scaled_original);
    g_object_unref(pixbuf_original);
    g_object_unref(pixbuf_scaled_original);

    GdkPixbuf* pixbuf_rotated = gdk_pixbuf_new_from_file("rotate_tmp.png", NULL);
    get_scaled_size(gdk_pixbuf_get_width(pixbuf_rotated), gdk_pixbuf_get_height(pixbuf_rotated),
                    400, 400, &new_width, &new_height);
    GdkPixbuf* pixbuf_scaled_rotated = gdk_pixbuf_scale_simple(
        pixbuf_rotated, new_width, new_height, GDK_INTERP_BILINEAR);
    GtkWidget* image_rotated = gtk_image_new_from_pixbuf(pixbuf_scaled_rotated);
    g_object_unref(pixbuf_rotated);
    g_object_unref(pixbuf_scaled_rotated);

    remove("rotate_tmp.png");
    free_image(img);
    free_image(original);

    /* Génére la boite horizontal pour aligner les objets. */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), image_original, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), image_rotated, TRUE, TRUE, 0);

    /* Bouton recommencer */
    GtkWidget *restart_button = gtk_button_new_with_label("Recommencer");
    g_signal_connect(restart_button, "clicked", G_CALLBACK(restart_clicked), window);

    /* Bouton rotation manuelle */
    GtkWidget *manual_button = gtk_button_new_with_label("Rotation Manuelle");
    g_signal_connect(manual_button, "clicked", G_CALLBACK(manual_rotation_clicked), window);

    /* Bouton Transmettre au réseau de neuronne */
    GtkWidget *neural_button = gtk_button_new_with_label("Transmettre au réseau");
    g_signal_connect(neural_button, "clicked", G_CALLBACK(nn_transf), window);

    GtkWidget *vbox_display = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox_display), hbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_display), restart_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_display), manual_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_display), neural_button, FALSE, FALSE, 0);

    gtk_window_set_default_size(GTK_WINDOW(window), 850, 450);

    gtk_container_add(GTK_CONTAINER(window), vbox_display);
    gtk_widget_show_all(window);

    g_free(text);
    cleanup_hidden_renderer_scale();
    cleanup_hidden_renderer();
    IMG_Quit();
    SDL_Quit();
}

/* Menu pour charger une nouvelle image. */
void restart_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Choisir une image",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Annuler", GTK_RESPONSE_CANCEL,
        "_Ouvrir", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        GtkEntry *tmp_entry = GTK_ENTRY(gtk_entry_new());
        gtk_entry_set_text(tmp_entry, filename);

        rotate_image(window, tmp_entry, 0.0);

        gtk_widget_destroy(GTK_WIDGET(tmp_entry));
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/* Menu pour la rotation manuel */
void manual_rotation_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window = GTK_WIDGET(user_data);
    const char *cur_filename = (const char *)g_object_get_data(G_OBJECT(window), "current-filename");
    if (!cur_filename)
    {
        g_print("Aucun fichier courant enregistré pour la rotation manuelle.\n");
        return;
    }
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Rotation Manuelle",
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            "_Annuler", GTK_RESPONSE_CANCEL,
            "_Valider", GTK_RESPONSE_ACCEPT,
            NULL
    );
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Entrez l'angle de rotation (en degrés) :");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    GtkWidget *angle_entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content_area), angle_entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        const gchar *angle_text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
        double angle = atof(angle_text);

        GtkEntry *tmp_entry = GTK_ENTRY(gtk_entry_new());
        gtk_entry_set_text(tmp_entry, cur_filename);

        rotate_image(window, tmp_entry, angle);

        gtk_widget_destroy(GTK_WIDGET(tmp_entry));
    }

    gtk_widget_destroy(dialog);

}

/* */
void validate_path(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);

    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    rotate_image(window, entry, 0.0);
}

/* path  */
void on_path_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Choisir une image",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Annuler", GTK_RESPONSE_CANCEL,
        "_Ouvrir", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(entry, filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/* Exit fonction */
void on_window_destroy(GtkWidget *widget, gpointer data) {
    remove_directory("./output");
    gtk_main_quit();
}

/* Fonction principal */
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sélection d'image");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 120);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Chemin vers l'image...");
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);

    GtkWidget *path_button = gtk_button_new_with_label("Path");
    gtk_box_pack_start(GTK_BOX(hbox), path_button, FALSE, FALSE, 0);

    GtkWidget *validate_button = gtk_button_new_with_label("Valider");
    gtk_box_pack_end(GTK_BOX(vbox), validate_button, FALSE, FALSE, 0);

    g_signal_connect(path_button, "clicked", G_CALLBACK(on_path_button_clicked), entry);
    g_signal_connect(validate_button, "clicked", G_CALLBACK(validate_path), entry);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

