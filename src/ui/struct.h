#pragma once
#include "../global.h"
#include "../core/image.h"
#include "../pre_process/grayscale.h"
#include "../pre_process/grid/grid_detection.h"
#include "../pre_process/rotation/rotation.h"
#include "../pre_process/scale.h"
#include "../pre_process/utils/shapes.h"
#include <time.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>


typedef struct {
    GtkWidget *window;
    Image *image;
    GtkWidget *entry;
    GtkWidget *gtkimg;
} CallbackData;
