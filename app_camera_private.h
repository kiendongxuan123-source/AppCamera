#ifndef APP_CAMERA_PRIVATE_H_
#define APP_CAMERA_PRIVATE_H_

#include "app_camera.h"

G_BEGIN_DECLS

typedef struct {
    /* --- PRIVATE: không đăng ký GObject property, chỉ có API get/set thường --- */
    gchar *id;
    gchar *rtsp_url;
    gchar *codec;

    /* --- PUBLIC: đăng ký GObject property đầy đủ --- */
    gchar *name;
    AppCameraStatus status;

    guint width;
    guint height;
    guint fps;

    gboolean is_enabled;
    gboolean is_recording;

    gdouble pan;
    gdouble tilt;
    gdouble zoom;
} AppCameraPrivate;

G_END_DECLS

#endif /* APP_CAMERA_PRIVATE_H_ */