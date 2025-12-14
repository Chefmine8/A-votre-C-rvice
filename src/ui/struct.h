#pragma once
#include "../core/image.h"
#include "../global.h"
#include "../link/link_cells_to_nn.h"
#include "../neural_network/layer.h"
#include "../neural_network/neural_network.h"
#include "../pre_process/grayscale.h"
#include "../pre_process/grid/grid_detection.h"
#include "../pre_process/rotation/rotation.h"
#include "../pre_process/scale.h"
#include "../pre_process/utils/shapes.h"
#include "../solver/solver.h"
#include <errno.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int create_directory_if_not_exists(const char *path);
int remove_directory(const char *path);
void restart_clicked(GtkButton *button, gpointer user_data);
void manual_rotation_clicked(GtkButton *button, gpointer user_data);
