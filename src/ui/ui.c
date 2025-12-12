#include "struct.h"


void rotate_image(GtkWidget *window, GtkEntry *entry) {
    const gchar *text_tmp = gtk_entry_get_text(entry);
    if (!text_tmp || *text_tmp == '\0') {
        g_print("No file path provided.\n");
        return;
    }
    gchar *text = g_strdup(text_tmp);


    /* Blank screen */
    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(iter->data);
    g_list_free(children);


    /* Show Image */
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    Image *img = load_image(text);

    Image *scale = resize_image(img, 430, 430, true);
    free_image(img);

    export_image(image_to_sdl_surface(scale), "rotate_tmp.png");

    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("rotate_tmp.png", NULL);
    GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);

    remove("rotate_tmp.png");

    grayscale_image(scale);

    sauvola(scale, 12, 128, 0.07);

    Shape **shapes = get_all_shape(scale);

    remove_small_shape(scale, shapes, 8);
    remove_outliers_shape(scale, shapes, 25, 75, 2.5, 3);
    remove_aspect_ration(scale, shapes, 0.1, 5);

    gtk_widget_show_all(window);
}

void validate_path(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);

    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    rotate_image(window, entry);
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

void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}


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

