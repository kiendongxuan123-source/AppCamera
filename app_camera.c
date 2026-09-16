#include "app_camera.h"
/* Khai báo Property ID để hỗ trợ g_object_get/set */
enum {
    PROP_0,
    PROP_NAME, PROP_WIDTH, PROP_FPS, PROP_STATUS,PROP_HEIGHT,PROP_PAN,PROP_TILT,PROP_IS_ENABLED,PROP_IS_RECORDING,PROP_ZOOM,        /* Public (Có thể phơi ra thêm nếu muốn dùng g_object_set) */
    N_PROPERTIES
};
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };
GType
app_camera_status_get_type (void)
{
  static GType type = 0;

  if (type == 0)
    {
      static const GEnumValue values[] = {
        { APP_CAMERA_STATUS_DISCONNECTED, "APP_CAMERA_STATUS_DISCONNECTED", "disconnected" },
        { APP_CAMERA_STATUS_CONNECTING,   "APP_CAMERA_STATUS_CONNECTING",   "connecting" },
        { APP_CAMERA_STATUS_IDLE,         "APP_CAMERA_STATUS_IDLE",         "idle" },
        { APP_CAMERA_STATUS_STREAMING,    "APP_CAMERA_STATUS_STREAMING",    "streaming" },
        { APP_CAMERA_STATUS_RECORDING,    "APP_CAMERA_STATUS_RECORDING",    "recording" },
        { APP_CAMERA_STATUS_ERROR,        "APP_CAMERA_STATUS_ERROR",        "error" },
        { 0, NULL, NULL }
      };
      type = g_enum_register_static ("AppCameraStatus", values);
    }
  return type;
}
/* Macro liên kết Class với Private Struct */
typedef struct
{
  /* private, chỉ có API get/set thường */
  gchar *id;
  gchar *rtsp_url;
  gchar *codec;

  /* "public" theo nghĩa property, nhưng vẫn nằm trong .c */
  gchar *name;
  AppCameraStatus status;
  guint width;
  guint height;
  guint fps;
  gboolean is_enabled;
  gboolean is_recording;
  gdouble pan, tilt, zoom;
} AppCameraPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AppCamera, app_camera, G_TYPE_OBJECT)  // vung nho cua AppCameraPrivate se duoc cap phat lien ke ngay sau AppCamera
// id la ten field trong struct AppCameraPrivate(no la con tro)
// priv la con tro kieu AppCameraPrivate , nam giu dia chia cua mot object AppCamera(chinh la self)
// priv->id lay gia tri trong o nho id bang cach cong offset địa chỉ và giá trị trong ô nhớ đó chính là một địa chỉ trỏ tới vùng nhớ chứa chuỗi từ đó ta có thể suy ra rằng priv->id cũng chính là một con trỏ( boi vi id cung la mot tro tro toi mot vung nho )
// &priv->id là địa chỉ của con trỏ priv->id 
// priv->width khác ở chỗ là vì width khoogn phải con trỏ cho nên priv->width chính xác là giá trị trong ô nhớ width của object luôn

                         // ham instance_private la ham tinh toan tinh toan dia chi de con cho priv co the tro dung vi tri
/* Hàm dọn dẹp bộ nhớ */
static void app_camera_finalize(GObject *object) {
    AppCamera *self = APP_CAMERA(object);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    
    /* Dọn Private Data */
    g_clear_pointer(&priv->id, g_free);
    g_clear_pointer(&priv->rtsp_url, g_free);
    g_clear_pointer(&priv->codec, g_free);

    /* Dọn Public Data */
    g_clear_pointer(&priv->name, g_free);
    
    
    
    G_OBJECT_CLASS(app_camera_parent_class)->finalize(object);
}

static void app_camera_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
    AppCamera *self = APP_CAMERA(object);
    

    switch (property_id) {

        /* Public Properties (Lấy từ struct Public) */
        case PROP_NAME:         g_value_set_string(value, app_camera_get_name(self)); break;
        case PROP_WIDTH:        g_value_set_uint(value, app_camera_get_width(self)); break;
        case PROP_HEIGHT:       g_value_set_uint(value, app_camera_get_height(self)); break;
        case PROP_FPS:          g_value_set_uint(value, app_camera_get_fps(self)); break;
        case PROP_STATUS:       g_value_set_enum(value, app_camera_get_status(self)); break;
        case PROP_PAN:          g_value_set_double(value, app_camera_get_pan(self)); break;
        case PROP_TILT:         g_value_set_double(value, app_camera_get_tilt(self)); break;
        case PROP_ZOOM:         g_value_set_double(value, app_camera_get_zoom(self)); break;
        case PROP_IS_ENABLED:   g_value_set_boolean(value, app_camera_get_enabled(self)); break;   // (hoặc get_is_enabled nếu chọn cách B ở trên)
        case PROP_IS_RECORDING: g_value_set_boolean(value, app_camera_get_recording(self)); break; // gan chuoi vao value

        default: 
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec); 
            break;
    }
}

static void
app_camera_set_property (GObject *object, guint property_id,
                          const GValue *value, GParamSpec *pspec)
{
  AppCamera *self = APP_CAMERA (object);

  switch (property_id) {
    case PROP_NAME:
      app_camera_set_name (self, g_value_get_string (value));
      break;
    case PROP_WIDTH:
      app_camera_set_width (self, g_value_get_uint (value));
      break;
    case PROP_HEIGHT:
      app_camera_set_height (self, g_value_get_uint (value));
      break;
    case PROP_FPS:
      app_camera_set_fps (self, g_value_get_uint (value));
      break;
    case PROP_STATUS:
      app_camera_set_status (self, g_value_get_enum (value));   // sửa luôn get_int -> get_enum
      break;
    case PROP_PAN:
      app_camera_set_pan (self, g_value_get_double (value));
      break;
    case PROP_TILT:
      app_camera_set_tilt (self, g_value_get_double (value));
      break;
    case PROP_ZOOM:
      app_camera_set_zoom (self, g_value_get_double (value));
      break;
    case PROP_IS_ENABLED:
      app_camera_set_enabled (self, g_value_get_boolean (value));
      break;
    case PROP_IS_RECORDING:
      app_camera_set_recording (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void app_camera_class_init(AppCameraClass *klass) {  // điền các giá trị khởi tạo vào bảng cũng như các hàm tương ứng với các con trỏ hàm
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    
    object_class->finalize = app_camera_finalize;  // con tro ham
    object_class->get_property = app_camera_get_property; // phai ghi de moi dung duoc ham g_object_class_install_properties va sau nay quan trong cho tính năng liên kết dữ liệu
    object_class->set_property = app_camera_set_property; // di toi dia chi ma object_class tro toi chinh la GObjectClass roi truy cap vao con tro ham set_property gan o nho set_property thanh dia chi ham kia
    // set_property la con tro ham gan dia chi cua ham app_camera_set_property object_class la con tro va la phuong tien de truy cap va con tro ham
    
    obj_properties[PROP_IS_ENABLED] = g_param_spec_boolean (
    "is-enabled",          /* 1. name: tên định danh chuẩn (thường xài dấu gạch ngang) */
    "Is Active",          /* 2. nickname: tên ngắn gọn */
    "Is camera active",   /* 3. blurb: mô tả chi tiết */
    FALSE,                /* 4. default_value: giá trị mặc định (TRUE hoặc FALSE) */
    G_PARAM_READWRITE  /* 5. flags: quyền đọc/ghi */
    );
    obj_properties[PROP_IS_RECORDING] = g_param_spec_boolean (
    "is-recording",          /* 1. name: tên định danh chuẩn (thường xài dấu gạch ngang) */
    "Is Recording",          /* 2. nickname: tên ngắn gọn */
    "Ghi hinh",   /* 3. blurb: mô tả chi tiết */
    FALSE,                /* 4. default_value: giá trị mặc định (TRUE hoặc FALSE) */
    G_PARAM_READWRITE  /* 5. flags: quyền đọc/ghi */
    );
    obj_properties[PROP_NAME] = g_param_spec_string (
    "name",            /* 1. name ten dinh danh chuan cua property */
    "Name",            /* 2. nickname ten ngan gon*/
    "Name Camera",     /* 3. blurb mo ta chi tiet */
    "Camera Test",            /* 4. default_value gia tri ban dau rong*/
    G_PARAM_READWRITE /* 5. flags doc va ghi*/
    );
    obj_properties[PROP_WIDTH] = g_param_spec_uint (
    "width",         /* 1. name */
    "Width",         /* 2. nick */
    "Resolution Width", /* 3. blurb */
    0,               /* 4. minimum */
    G_MAXUINT,            /* 5. maximum */
    1920,            /* 6. default_value gia tri ban dau*/
    G_PARAM_READWRITE /* 7. flags */
    );
    obj_properties[PROP_STATUS] = g_param_spec_enum (
    "status",
    "Status",
    "Camera Status",
    APP_TYPE_CAMERA_STATUS,             /* GType của enum, thay vì min/max */
    APP_CAMERA_STATUS_DISCONNECTED,     /* Default */
    G_PARAM_READWRITE
    );
    obj_properties[PROP_HEIGHT] = g_param_spec_uint (
    "height",         /* 1. name */
    "Height",         /* 2. nick */
    "Resolution Height", /* 3. blurb */
    0,               /* 4. minimum */
    G_MAXUINT,            /* 5. maximum */
    1080,            /* 6. default_value gia tri ban dau*/
    G_PARAM_READWRITE /* 7. flags */
    );
    obj_properties[PROP_FPS] = g_param_spec_uint (
    "fps",         /* 1. name */
    "Fps",         /* 2. nick */
    "Toc do khung hinh", /* 3. blurb */
    1,               /* 4. minimum */
    G_MAXUINT,            /* 5. maximum */
    30,            /* 6. default_value gia tri ban dau*/
    G_PARAM_READWRITE /* 7. flags */
    );
    obj_properties[PROP_PAN] = g_param_spec_double (
    "pan",
    "Pan Angle",
    "Horizontal Pan Angle in degrees",
    -G_MAXDOUBLE,
    G_MAXDOUBLE,
    0.0,
    G_PARAM_READWRITE
    );
    obj_properties[PROP_TILT] = g_param_spec_double (
    "tilt",
     "Tilt Angle", 
     "Vertical Tilt Angle in degrees",
    -G_MAXDOUBLE,
     G_MAXDOUBLE,
      0.0,
    G_PARAM_READWRITE
    );
    obj_properties[PROP_ZOOM] = g_param_spec_double (
    "zoom",
    "ZOOM", 
    "Phongto",
    -G_MAXDOUBLE,
    G_MAXDOUBLE,
    1.0,
    G_PARAM_READWRITE
    );

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void app_camera_init(AppCamera *self) {
    /* Khởi tạo giá trị mặc định cho Public Data */
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    priv->name = g_strdup("Camera Test");
    priv->status= APP_CAMERA_STATUS_DISCONNECTED;
    priv->width = 1920;
    priv->height = 1080;
    priv->fps = 30;
    priv->pan = 0.0;
    priv->tilt= 0.0;
    priv->zoom=1.0;
    priv->is_recording = FALSE;
    priv->is_enabled = FALSE; // tat camera
    /* Khởi tạo giá trị mặc định cho Private Data */
    priv->id = NULL;             // hoặc g_strdup("") nếu muốn chuỗi rỗng thay vì NULL
    priv->rtsp_url = NULL;
    priv->codec=NULL;
}

AppCamera* app_camera_new(void) {
    return g_object_new(APP_TYPE_CAMERA, NULL);
}


const gchar* app_camera_get_id(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), NULL);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->id;
}
void app_camera_set_id(AppCamera *self, const gchar *id) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    
    if (g_strcmp0(priv->id, id) != 0) {
        g_free(priv->id);
        priv->id = g_strdup(id);
    }
}


const gchar* app_camera_get_codec(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), NULL);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->codec;
}
void app_camera_set_codec(AppCamera *self, const gchar *codec) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    
    if (g_strcmp0(priv->codec, codec) != 0) {
        g_free(priv->codec);
        priv->codec = g_strdup(codec);
    }
}


const gchar* app_camera_get_rtsp_url(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), NULL);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->rtsp_url;
}
void app_camera_set_rtsp_url(AppCamera *self, const gchar *rtsp_url) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    
    if (g_strcmp0(priv->rtsp_url, rtsp_url) != 0) {
        g_free(priv->rtsp_url);
        priv->rtsp_url = g_strdup(rtsp_url);
    }
}


//Public
void app_camera_set_status(AppCamera *self, AppCameraStatus status) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    if (priv->status != status) {
        priv->status = status;

        /* Báo cho hệ thống GObject biết trạng thái đã đổi */
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_STATUS]);
    }
}

AppCameraStatus app_camera_get_status(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), APP_CAMERA_STATUS_DISCONNECTED);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->status;
}


guint app_camera_get_width(AppCamera *self) {
    /* 1. Kiểm tra an toàn: Nếu self == NULL hoặc không phải AppCamera, dừng và trả về 0 */
    g_return_val_if_fail(APP_IS_CAMERA(self), 0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* 2. Trả về giá trị từ private struct */
    return priv->width;
}

void app_camera_set_width(AppCamera *self, guint width) {
    /* 1. Kiểm tra an toàn con trỏ */
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* 2. Kiểm tra/Giới hạn dải giá trị (Clamp value: 0 - 4000) */
    guint new_width = MIN(width, 4000);

    /* 3. Chỉ thực hiện ghi nếu giá trị mới KHÁC giá trị hiện tại */
    if (priv->width != new_width) {
        priv->width = new_width;

        /* 4. Phát tín hiệu thông báo cho GObject Framework biết thuộc tính "width" đã thay đổi */
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_WIDTH]); // phat signal
    }
}


guint app_camera_get_height(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), 0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->height;
}

void app_camera_set_height(AppCamera *self, guint height) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    guint new_height = MIN(height, 2200);

    if (priv->height != new_height) {
        priv->height = new_height;
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_HEIGHT]);
    }
}


guint app_camera_get_fps(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), 0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->fps;
}

void app_camera_set_fps(AppCamera *self, guint fps) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* Bị chặn dưới bởi 1 và chặn trên bởi 120 */
    guint new_fps = MAX(1, MIN(fps, 120));

    if (priv->fps != new_fps) {
        priv->fps = new_fps;
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_FPS]);
    }
}


gdouble app_camera_get_aspect_ratio(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), 0.0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* Tránh lỗi chia cho 0 nếu height chưa được đặt */
    if (priv->height == 0) {
        return 0.0;
    }

    return (gdouble)priv->width / priv->height;
}


gboolean app_camera_get_enabled(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), FALSE);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->is_enabled;
}

void app_camera_set_enabled(AppCamera *self, gboolean enabled) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* Chuẩn hóa giá trị truyền vào về dạng 0 (FALSE) hoặc 1 (TRUE) */
    enabled = !!enabled;

    if (priv->is_enabled != enabled) {
        priv->is_enabled = enabled;
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_IS_ENABLED]);
    }
}


gboolean app_camera_get_recording(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), FALSE);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->is_recording;
}

void app_camera_set_recording(AppCamera *self, gboolean recording) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    recording = !!recording;

    if (priv->is_recording != recording) {
        priv->is_recording = recording;
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_IS_RECORDING]);
    }
}


gdouble app_camera_get_pan(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), 0.0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->pan;
}

void app_camera_set_pan(AppCamera *self, gdouble pan) {
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* Khống chế góc quay trong dải an toàn [-180.0, 180.0] */
    gdouble new_pan = CLAMP(pan, -180.0, 180.0);

    if (priv->pan != new_pan) {
        priv->pan = new_pan;
        /* Gửi lệnh điều khiển phần cứng/mô-tơ thực tế ở đây nếu có */
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_PAN]);
    }
}


gdouble app_camera_get_tilt(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), 0.0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->tilt;
}

void app_camera_set_tilt(AppCamera *self, gdouble tilt) {
    g_return_if_fail(APP_IS_CAMERA(self));  // macro kiem tra co dung la thuoc appCamera hay khong
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* Khống chế góc quay trong dải an toàn [-90.0, 90.0] */
    gdouble new_tilt = CLAMP(tilt, -90.0, 90.0);

    if (priv->tilt != new_tilt) {
        priv->tilt = new_tilt;
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_TILT]);
    }
}


gdouble app_camera_get_zoom(AppCamera *self) {
    g_return_val_if_fail(APP_IS_CAMERA(self), 0.0);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);
    return priv->zoom;
}

void app_camera_set_zoom(AppCamera *self, gdouble zoom) {
    g_return_if_fail(APP_IS_CAMERA(self));  // macro kiem tra co dung la thuoc appCamera hay khong
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* Khống chế trong dải an toàn [0.0, 5.0] */
    gdouble new_zoom = CLAMP(zoom, 0.0, 5.0);

    if (priv->zoom != new_zoom) {
        priv->zoom = new_zoom;
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_ZOOM]);
    }
}


const gchar* app_camera_get_name(AppCamera *self) {
    /* 1. Kiểm tra an toàn: Nếu self NULL hoặc không phải AppCamera, trả về NULL */
    g_return_val_if_fail(APP_IS_CAMERA(self), NULL);
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* 2. Trả về con trỏ chuỗi name nằm trong private struct */
    return priv->name;
}

void app_camera_set_name(AppCamera *self, const gchar *name) {
    /* 1. Kiểm tra an toàn con trỏ */
    g_return_if_fail(APP_IS_CAMERA(self));
    AppCameraPrivate *priv = app_camera_get_instance_private(self);

    /* 2. Chỉ thực hiện cập nhật nếu tên mới KHÁC tên hiện tại */
    /* g_strcmp0 an toàn với cả trường hợp NULL */
    if (g_strcmp0(priv->name, name) != 0) {
        /* Giải phóng chuỗi cũ */
        g_free(priv->name);

        /* Tạo bản sao chuỗi mới cấp phát động (Heap) */
        priv->name = g_strdup(name);

        /* 3. Báo cho GObject Framework biết thuộc tính "name" đã thay đổi */
        g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_NAME]);
    }
}
