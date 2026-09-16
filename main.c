#include "app_camera.h"
#include <glib.h>
#include <glib-object.h>
#include <stdio.h>
#include <math.h>

#define RESULTS_FILE "test_results.txt"

static FILE *g_results_file = NULL;
static gint  g_total  = 0;
static gint  g_passed = 0;

/* ===================================================================
 * HA TANG TEST: ghi ket qua ra file + in console
 * =================================================================== */
static void
record_result (const gchar *test_name, gboolean passed, const gchar *detail)
{
    g_total++;
    if (passed) g_passed++;

    fprintf (g_results_file, "%s|%s|%s\n",
             test_name, passed ? "PASS" : "FAIL", detail ? detail : "");
    g_print ("[%s] %s %s\n", passed ? "PASS" : "FAIL", test_name, detail ? detail : "");
}
#define CHECK(name, cond, detail) record_result((name), (cond), (detail))


/* ===================================================================
 * 1. DEFAULT VALUES
 * =================================================================== */
static void
test_default_values (void)
{
    g_print ("\n--- 1. Default values ---\n");
    AppCamera *cam = app_camera_new ();

    CHECK ("default_name",      g_strcmp0 (app_camera_get_name (cam), "Camera Test") == 0, "expect 'Camera Test'"); // g_strcmp0 la ham so sanh an toan voi NULl
    CHECK ("default_status",    app_camera_get_status (cam) == APP_CAMERA_STATUS_DISCONNECTED, "expect DISCONNECTED");
    CHECK ("default_width",     app_camera_get_width (cam) == 1920,  "expect 1920");
    CHECK ("default_height",    app_camera_get_height (cam) == 1080, "expect 1080");
    CHECK ("default_fps",       app_camera_get_fps (cam) == 30,      "expect 30");
    CHECK ("default_pan",       app_camera_get_pan (cam) == 0.0,     "expect 0.0");
    CHECK ("default_tilt",      app_camera_get_tilt (cam) == 0.0,    "expect 0.0");
    CHECK ("default_zoom",      app_camera_get_zoom (cam) == 1.0,    "expect 1.0");
    CHECK ("default_enabled",   app_camera_get_enabled (cam) == FALSE,   "expect FALSE");
    CHECK ("default_recording", app_camera_get_recording (cam) == FALSE, "expect FALSE");
    CHECK ("default_id_null",   app_camera_get_id (cam) == NULL, "chua set -> phai NULL");

    g_object_unref (cam);
}


/* ===================================================================
 * 2. GETTER / SETTER (gia tri hop le, binh thuong)
 * =================================================================== */
static void
test_getter_setter (void)
{
    g_print ("\n--- 2. Getter/setter ---\n");
    AppCamera *cam = app_camera_new ();

    app_camera_set_name (cam, "Cong chinh");
    CHECK ("set_get_name", g_strcmp0 (app_camera_get_name (cam), "Cong chinh") == 0, "");

    app_camera_set_width (cam, 1280);
    CHECK ("set_get_width", app_camera_get_width (cam) == 1280, "");

    app_camera_set_height (cam, 720);
    CHECK ("set_get_height", app_camera_get_height (cam) == 720, "");

    app_camera_set_fps (cam, 25);
    CHECK ("set_get_fps", app_camera_get_fps (cam) == 25, "");

    app_camera_set_status (cam, APP_CAMERA_STATUS_STREAMING);
    CHECK ("set_get_status", app_camera_get_status (cam) == APP_CAMERA_STATUS_STREAMING, "");

    app_camera_set_pan (cam, 45.0);
    CHECK ("set_get_pan", app_camera_get_pan (cam) == 45.0, "");

    app_camera_set_tilt (cam, -30.0);
    CHECK ("set_get_tilt", app_camera_get_tilt (cam) == -30.0, "");

    app_camera_set_zoom (cam, 2.5);
    CHECK ("set_get_zoom", app_camera_get_zoom (cam) == 2.5, "");

    app_camera_set_enabled (cam, TRUE);
    CHECK ("set_get_enabled", app_camera_get_enabled (cam) == TRUE, "");

    app_camera_set_recording (cam, TRUE);
    CHECK ("set_get_recording", app_camera_get_recording (cam) == TRUE, "");

    app_camera_set_id (cam, "CAM-001");
    CHECK ("set_get_id", g_strcmp0 (app_camera_get_id (cam), "CAM-001") == 0, "");

    app_camera_set_rtsp_url (cam, "rtsp://192.168.1.1/stream");
    CHECK ("set_get_rtsp_url", g_strcmp0 (app_camera_get_rtsp_url (cam), "rtsp://192.168.1.1/stream") == 0, "");

    app_camera_set_codec (cam, "H265");
    CHECK ("set_get_codec", g_strcmp0 (app_camera_get_codec (cam), "H265") == 0, "");

    app_camera_set_width (cam, 1920);
    app_camera_set_height (cam, 1080);
    gdouble ar = app_camera_get_aspect_ratio (cam);
    CHECK ("aspect_ratio", fabs (ar - (1920.0 / 1080.0)) < 0.0001, "1920x1080 -> ~1.777");

    g_object_unref (cam);
}


/* ===================================================================
 * 3. CLAMP BOUNDARY (gia tri vuot bien / dung tai bien)
 * =================================================================== */
static void
test_clamp_boundary (void)
{
    g_print ("\n--- 3. Clamp boundary ---\n");
    AppCamera *cam = app_camera_new ();

    /* Vuot xa bien */
    app_camera_set_width (cam, 999999);
    CHECK ("clamp_width_far_over", app_camera_get_width (cam) == 4000, "vuot xa max -> ve 4000");

    app_camera_set_height (cam, 999999);
    CHECK ("clamp_height_far_over", app_camera_get_height (cam) == 2200, "vuot xa max -> ve 2200");

    app_camera_set_fps (cam, 0);
    CHECK ("clamp_fps_far_under", app_camera_get_fps (cam) == 1, "duoi min -> ve 1");

    app_camera_set_fps (cam, 500);
    CHECK ("clamp_fps_far_over", app_camera_get_fps (cam) == 120, "vuot xa max -> ve 120");

    app_camera_set_pan (cam, 999.0);
    CHECK ("clamp_pan_far_over", app_camera_get_pan (cam) == 180.0, "");
    app_camera_set_pan (cam, -999.0);
    CHECK ("clamp_pan_far_under", app_camera_get_pan (cam) == -180.0, "");

    app_camera_set_tilt (cam, 999.0);
    CHECK ("clamp_tilt_far_over", app_camera_get_tilt (cam) == 90.0, "");
    app_camera_set_tilt (cam, -999.0);
    CHECK ("clamp_tilt_far_under", app_camera_get_tilt (cam) == -90.0, "");

    app_camera_set_zoom (cam, 999.0);
    CHECK ("clamp_zoom_far_over", app_camera_get_zoom (cam) == 5.0, "");
    app_camera_set_zoom (cam, -999.0);
    CHECK ("clamp_zoom_far_under", app_camera_get_zoom (cam) == 0.0, "");

    /* Dung tai bien - edge case, hay bi loi off-by-one */
    app_camera_set_fps (cam, 120);
    CHECK ("clamp_fps_exact_max", app_camera_get_fps (cam) == 120, "dung bang max -> giu nguyen");
    app_camera_set_fps (cam, 1);
    CHECK ("clamp_fps_exact_min", app_camera_get_fps (cam) == 1, "dung bang min -> giu nguyen");

    g_object_unref (cam);
}


/* ===================================================================
 * 4. PROPERTY API (g_object_set / g_object_get)
 * =================================================================== */
static void
test_property_api (void)
{
    g_print ("\n--- 4. Property API ---\n");
    AppCamera *cam = app_camera_new ();

    g_object_set (cam,
                  "name", "Property Cam",
                  "width", 800,
                  "height", 600,
                  "fps", 15,
                  "status", APP_CAMERA_STATUS_IDLE,
                  "pan", 10.0,
                  "tilt", 5.0,
                  "zoom", 1.5,
                  "is-enabled", TRUE,
                  "is-recording", FALSE,
                  NULL);

    gchar *name = NULL;
    guint width = 0, height = 0, fps = 0;
    AppCameraStatus status = 0;
    gdouble pan = 0, tilt = 0, zoom = 0;
    gboolean enabled = FALSE, recording = FALSE;

    g_object_get (cam,
                  "name", &name,
                  "width", &width,
                  "height", &height,
                  "fps", &fps,
                  "status", &status,
                  "pan", &pan,
                  "tilt", &tilt,
                  "zoom", &zoom,
                  "is-enabled", &enabled,
                  "is-recording", &recording,
                  NULL);

    CHECK ("prop_api_name",      g_strcmp0 (name, "Property Cam") == 0, "");
    CHECK ("prop_api_width",     width == 800, "");
    CHECK ("prop_api_height",    height == 600, "");
    CHECK ("prop_api_fps",       fps == 15, "");
    CHECK ("prop_api_status",    status == APP_CAMERA_STATUS_IDLE, "");
    CHECK ("prop_api_pan",       pan == 10.0, "");
    CHECK ("prop_api_tilt",      tilt == 5.0, "");
    CHECK ("prop_api_zoom",      zoom == 1.5, "");
    CHECK ("prop_api_enabled",   enabled == TRUE, "");
    CHECK ("prop_api_recording", recording == FALSE, "");

    /* Clamp phai dung du di qua Property API, khong chi qua ham C truc tiep */
    guint fps2 = 0;
    g_object_set (cam, "fps", 999, NULL);
    g_object_get (cam, "fps", &fps2, NULL);
    CHECK ("prop_api_clamp_applies", fps2 == 120, "clamp phai ap dung du di duong nao");

    g_free (name);
    g_object_unref (cam);
}


/* ===================================================================
 * 5. SIGNAL / NOTIFY
 * =================================================================== */
static gint  notify_count = 0;
static gchar *last_notified_prop = NULL;

static void
on_notify (GObject *object, GParamSpec *pspec, gpointer user_data)  // ham nay duoc kich hoat boi g_object_notify_by_pspec
{
    (void) object; (void) user_data;
    notify_count++;
    g_free (last_notified_prop);
    last_notified_prop = g_strdup (g_param_spec_get_name (pspec));
}

static void
test_notify_signal (void)
{
    g_print ("\n--- 5. Signal/notify ---\n");
    AppCamera *cam = app_camera_new ();
    g_signal_connect (cam, "notify::width", G_CALLBACK (on_notify), NULL); // dang ky notify

    notify_count = 0;
    app_camera_set_width (cam, 1280);
    CHECK ("notify_fires_on_change", notify_count == 1, "gia tri doi -> phai bao 1 lan");
    CHECK ("notify_correct_prop_name", g_strcmp0 (last_notified_prop, "width") == 0, "");

    notify_count = 0;
    app_camera_set_width (cam, 1280);   /* set trung gia tri cu */
    CHECK ("notify_silent_on_same_value", notify_count == 0, "khong doi -> khong duoc bao");

    notify_count = 0;
    g_object_set (cam, "width", 640, NULL);   /* di qua Property API */
    CHECK ("notify_fires_via_property_api", notify_count == 1, "");

    g_object_unref (cam);
}


/* ===================================================================
 * 6. HAI CAMERA DOC LAP - moi camera tu ghi state ra file rieng
 * =================================================================== */
static void
save_camera_state (AppCamera *cam, const gchar *filepath)
{
    FILE *f = fopen (filepath, "w");
    if (!f) return;
    fprintf (f, "name=%s\n",  app_camera_get_name (cam));
    fprintf (f, "width=%u\n", app_camera_get_width (cam));
    fprintf (f, "fps=%u\n",   app_camera_get_fps (cam));
    fclose (f);
}

static gchar *
read_field_from_file (const gchar *filepath, const gchar *key)
{
    gchar *contents = NULL;
    if (!g_file_get_contents (filepath, &contents, NULL, NULL))
        return NULL;

    gchar *result = NULL;
    gchar **lines = g_strsplit (contents, "\n", -1);
    for (gint i = 0; lines[i] != NULL; i++) {
        if (g_str_has_prefix (lines[i], key)) {
            gchar **kv = g_strsplit (lines[i], "=", 2);
            if (kv[0] && kv[1])
                result = g_strdup (kv[1]);
            g_strfreev (kv);
            break;
        }
    }
    g_strfreev (lines);
    g_free (contents);
    return result;
}

static void
test_two_independent_cameras (void)
{
    g_print ("\n--- 6. Hai camera doc lap ---\n");
    AppCamera *cam_a = app_camera_new ();
    AppCamera *cam_b = app_camera_new ();

    app_camera_set_name (cam_a, "Camera A");
    app_camera_set_width (cam_a, 1920);
    app_camera_set_fps (cam_a, 60);

    app_camera_set_name (cam_b, "Camera B");
    app_camera_set_width (cam_b, 640);
    app_camera_set_fps (cam_b, 15);

    /* Kiem tra ngay tren object, truoc khi ghi file */
    CHECK ("independent_diff_ptr", cam_a != cam_b, "hai dia chi bo nho khac nhau");
    CHECK ("independent_name_a_live", g_strcmp0 (app_camera_get_name (cam_a), "Camera A") == 0, "");
    CHECK ("independent_name_b_live", g_strcmp0 (app_camera_get_name (cam_b), "Camera B") == 0, "");

    /* Moi camera ghi state ra FILE RIENG cua no */
    save_camera_state (cam_a, "cam_a_state.txt");
    save_camera_state (cam_b, "cam_b_state.txt");

    /* Huy object - chi con file la "bang chung" duy nhat con lai */
    g_object_unref (cam_a);
    g_object_unref (cam_b);

    /* Doc lai tu 2 file khac nhau, doc lap voi bo nho object da mat */
    gchar *name_a = read_field_from_file ("cam_a_state.txt", "name");
    gchar *name_b = read_field_from_file ("cam_b_state.txt", "name");
    gchar *width_a = read_field_from_file ("cam_a_state.txt", "width");
    gchar *width_b = read_field_from_file ("cam_b_state.txt", "width");

    CHECK ("file_cam_a_name", g_strcmp0 (name_a, "Camera A") == 0, "");
    CHECK ("file_cam_b_name", g_strcmp0 (name_b, "Camera B") == 0, "");
    CHECK ("file_cam_a_width", g_strcmp0 (width_a, "1920") == 0, "");
    CHECK ("file_cam_b_width", g_strcmp0 (width_b, "640") == 0, "");
    CHECK ("files_not_identical", g_strcmp0 (name_a, name_b) != 0,
           "2 file phai khac nhau -> khong bi lan du lieu giua 2 object");

    g_free (name_a); g_free (name_b);
    g_free (width_a); g_free (width_b);
}


/* ===================================================================
 * 7. LIFECYCLE create/unref
 * =================================================================== */
static void
test_lifecycle (void)
{
    g_print ("\n--- 7. Lifecycle create/unref ---\n");
    AppCamera *cam = app_camera_new ();
    gpointer weak_ptr = cam;

    CHECK ("lifecycle_create_ok", cam != NULL, "");
    CHECK ("lifecycle_refcount_initial", G_OBJECT (cam)->ref_count == 1, "refcount = 1 sau khi new");

    /* GLib tu dong gan NULL vao weak_ptr khi object THUC SU bi finalize */
    g_object_add_weak_pointer (G_OBJECT (cam), &weak_ptr);

    g_object_unref (cam);

    CHECK ("lifecycle_finalized_after_unref", weak_ptr == NULL,
           "object phai thuc su bi huy, khong chi giam refcount");
}


/* ===================================================================
 * PHAN DOC LAI FILE KET QUA (giai doan doc lap voi giai doan chay test)
 * =================================================================== */
static void
verify_results_file (const gchar *path)
{
    gchar *contents = NULL;
    gsize  len = 0;
    GError *error = NULL;

    if (!g_file_get_contents (path, &contents, &len, &error)) {
        g_printerr ("Khong doc duoc file ket qua: %s\n", error->message);
        g_error_free (error);
        return;
    }

    gchar **lines = g_strsplit (contents, "\n", -1);
    gint pass = 0, fail = 0, total = 0;

    for (gint i = 0; lines[i] != NULL; i++) {
        if (lines[i][0] == '\0') continue;
        gchar **fields = g_strsplit (lines[i], "|", 3);
        if (fields[0] && fields[1]) {
            total++;
            if (g_strcmp0 (fields[1], "PASS") == 0) {
                pass++;
            } else {
                fail++;
                g_print ("  -> [doc lai tu file] FAIL: %s (%s)\n",
                         fields[0], fields[2] ? fields[2] : "");
            }
        }
        g_strfreev (fields);
    }

    g_print ("\n=== Doc lai '%s': %d/%d PASS, %d FAIL ===\n", path, pass, total, fail);

    g_strfreev (lines);
    g_free (contents);
}


/* ===================================================================
 * MAIN
 * =================================================================== */
int
main (void)
{
    g_results_file = fopen (RESULTS_FILE, "w");
    if (!g_results_file) {
        g_printerr ("Khong mo duoc file ghi ket qua\n");
        return 1;
    }

    g_print ("=== BAT DAU TEST APPCAMERA ===\n");

    test_default_values ();
    test_getter_setter ();
    test_clamp_boundary ();
    test_property_api ();
    test_notify_signal ();
    test_two_independent_cameras ();
    test_lifecycle ();

    fclose (g_results_file);

    g_print ("\n=== KET QUA CHAY TRUC TIEP: %d/%d PASS ===\n", g_passed, g_total);

    verify_results_file (RESULTS_FILE);

    g_free (last_notified_prop);
    return (g_passed == g_total) ? 0 : 1;
}