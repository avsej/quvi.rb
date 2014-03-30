/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <quvi.h>
#pragma GCC diagnostic pop

#include <ruby.h>
#include <ruby/thread.h>

VALUE qv_mQuvi;
VALUE qv_cHandle;
VALUE qv_eError;

VALUE qv_DEFAULT_USER_AGENT;

VALUE qv_sym_any;
VALUE qv_sym_autoproxy;
VALUE qv_sym_media;
VALUE qv_sym_online;
VALUE qv_sym_playlist;
VALUE qv_sym_subtitle;
VALUE qv_sym_type;
VALUE qv_sym_user_agent;
VALUE qv_sym_export_format;
VALUE qv_sym_filepath;
VALUE qv_sym_domains;
VALUE qv_sym_sha1;
VALUE qv_sym_title;
VALUE qv_sym_id;
VALUE qv_sym_thumbnail_url;
VALUE qv_sym_start_time_ms;
VALUE qv_sym_duration_ms;
VALUE qv_sym_video_height;
VALUE qv_sym_video_width;
VALUE qv_sym_video_encoding;
VALUE qv_sym_video_bitrate_kbit_s;
VALUE qv_sym_audio_encoding;
VALUE qv_sym_audio_bitrate_kbit_s;
VALUE qv_sym_container;
VALUE qv_sym_url;
VALUE qv_sym_streams;
VALUE qv_sym_best;

typedef struct qv_handle_st
{
    quvi_t q;
} qv_handle_t;


static void qv_handle_mark(void *ptr)
{
    qv_handle_t *handle = ptr;
    (void)handle;
}

static void qv_handle_free(void *ptr)
{
    qv_handle_t *handle = ptr;
    if (handle) {
        quvi_free(handle->q);
        handle->q = NULL;
    }
}

#define qv_raise(handle, message) qv_raise_at(handle, message, __FILE__, __LINE__)
static void qv_raise_at(qv_handle_t *handle, VALUE message, const char *file, int line)
{
    VALUE exc, str;
    const char *qv_msg;
    char buf[10];
    int qv_code;

    str = rb_str_new_shared(message);
    qv_msg = quvi_errmsg(handle->q);
    qv_code = quvi_errcode(handle->q);
    if (qv_msg) {
        snprintf(buf, 10, ": (0x%02x) \"", qv_code);
        rb_str_buf_cat_ascii(str, buf);
        rb_str_buf_cat_ascii(str, qv_msg);
        rb_str_buf_cat_ascii(str, "\" ");
    }
    rb_str_buf_cat_ascii(str, " at ");
    rb_str_buf_cat_ascii(str, file);
    snprintf(buf, 10, ":%d", line);
    rb_str_buf_cat_ascii(str, buf);
    exc = rb_exc_new3(qv_eError, str);
    rb_ivar_set(exc, rb_intern("@code"), INT2FIX(qv_code));
    rb_exc_raise(exc);
}

VALUE qv_handle_alloc(VALUE klass)
{
    VALUE obj;
    qv_handle_t *handle;
    obj = Data_Make_Struct(klass, qv_handle_t, qv_handle_mark, qv_handle_free, handle);
    return obj;
}

VALUE qv_handle_init(int argc, VALUE *argv, VALUE self)
{
    qv_handle_t *handle = DATA_PTR(self);
    VALUE opts,
          user_agent = qv_DEFAULT_USER_AGENT,
          autoproxy = Qfalse;

    rb_scan_args(argc, argv, "01", &opts);
    if (opts != Qnil) {
        VALUE arg;
        Check_Type(opts, T_HASH);
        arg = rb_hash_lookup2(opts, qv_sym_autoproxy, Qundef);
        if (arg != Qundef) {
            autoproxy = arg;
        }
        arg = rb_hash_aref(opts, qv_sym_user_agent);
        if (arg != Qnil) {
            Check_Type(arg, T_STRING);
            user_agent = arg;
        }
    }
    handle->q = quvi_new();
    if (quvi_ok(handle->q) == QUVI_FALSE) {
        qv_raise(handle, rb_str_new_cstr("unable create quvi_t handle"));
    }
    rb_funcall2(self, rb_intern("autoproxy="), 1, &autoproxy);
    rb_funcall2(self, rb_intern("user_agent="), 1, &user_agent);
    return self;
}

VALUE qv_handle_autoproxy_set(VALUE self, VALUE val)
{
    qv_handle_t *handle = DATA_PTR(self);
    int new_val = RTEST(val);

    quvi_set(handle->q, QUVI_OPTION_AUTOPROXY, new_val);
    rb_ivar_set(self, rb_intern("@autoproxy"), new_val ? Qtrue : Qfalse);
    return new_val;
}

VALUE qv_handle_user_agent_set(VALUE self, VALUE val)
{
    qv_handle_t *handle = DATA_PTR(self);

    Check_Type(val, T_STRING);
    quvi_set(handle->q, QUVI_OPTION_USER_AGENT, RSTRING_PTR(val));
    rb_ivar_set(self, rb_intern("@user_agent"), val);
    return val;
}

typedef struct qv_supports_params_st
{
    qv_handle_t *handle;
    char *url;
    QuviSupportsType type;
    QuviSupportsMode mode;
    QuviBoolean res;
} qv_supports_params_t;

static void* qv_handle_supports_p_nogvl(void *data1)
{
    qv_supports_params_t *params = data1;
    params->res = quvi_supports(params->handle->q, params->url,
                                params->mode, params->type);
    return 0;
}

VALUE qv_handle_supports_p(int argc, VALUE *argv, VALUE self)
{
    VALUE opts, url;
    qv_supports_params_t params;

    rb_scan_args(argc, argv, "11", &url, &opts);

    params.handle  = DATA_PTR(self);
    params.type = QUVI_SUPPORTS_TYPE_ANY;
    params.mode = QUVI_SUPPORTS_MODE_OFFLINE;
    params.res = QUVI_FALSE;
    params.url = StringValueCStr(url);
    if (opts != Qnil) {
        VALUE arg;
        Check_Type(opts, T_HASH);
        arg = rb_hash_lookup2(opts, qv_sym_online, Qundef);
        if (arg != Qundef) {
            params.mode = RTEST(arg) ? QUVI_SUPPORTS_MODE_ONLINE : QUVI_SUPPORTS_MODE_OFFLINE;
        }
        arg = rb_hash_aref(opts, qv_sym_type);
        if (arg != Qnil) {
            if (arg == qv_sym_media) {
                params.type = QUVI_SUPPORTS_TYPE_MEDIA;
            } else if (arg == qv_sym_playlist) {
                params.type = QUVI_SUPPORTS_TYPE_PLAYLIST;
            } else if (arg == qv_sym_subtitle) {
                params.type = QUVI_SUPPORTS_TYPE_SUBTITLE;
            } else if (arg == qv_sym_any) {
                params.type = QUVI_SUPPORTS_TYPE_ANY;
            } else {
                rb_raise(rb_eArgError, "unknown type: %s", StringValueCStr(arg));
            }
        }
    }
    rb_thread_call_without_gvl(qv_handle_supports_p_nogvl, &params, RUBY_UBF_IO, 0);
    if (params.res == QUVI_TRUE) {
        return Qtrue;
    }
    if (quvi_errcode(params.handle->q) != QUVI_ERROR_NO_SUPPORT) {
        qv_raise(params.handle, rb_sprintf("unable to check if URL supported (url=%s)", params.url));
    }
    return Qfalse;
}

VALUE qv_handle_each_script(VALUE self)
{
    qv_handle_t *handle = DATA_PTR(self);
    QuviScriptType types[] = {
        QUVI_SCRIPT_TYPE_SCAN,
        QUVI_SCRIPT_TYPE_PLAYLIST,
        QUVI_SCRIPT_TYPE_MEDIA,
        QUVI_SCRIPT_TYPE_SUBTITLE,
        QUVI_SCRIPT_TYPE_SUBTITLE_EXPORT
    };
    VALUE type_names[] = {
        rb_str_freeze(rb_external_str_new_cstr("scan")),
        rb_str_freeze(rb_external_str_new_cstr("playlist")),
        rb_str_freeze(rb_external_str_new_cstr("media")),
        rb_str_freeze(rb_external_str_new_cstr("subtitle")),
        rb_str_freeze(rb_external_str_new_cstr("subtitle_export"))
    };
    size_t i;

    RETURN_ENUMERATOR(self, 0, NULL);

    for (i = 0; i < sizeof(types) / sizeof(QuviScriptType); ++i) {
        while (quvi_script_next(handle->q, types[i]) == QUVI_TRUE) {
            VALUE script = rb_hash_new();
            char *s;

#define set_property(key, id) \
            s = NULL; \
            quvi_script_get(handle->q, types[i], id, &s); \
            if (s) { rb_hash_aset(script, key, rb_external_str_new_cstr(s)); }
            set_property(qv_sym_sha1, QUVI_SCRIPT_PROPERTY_SHA1);
            set_property(qv_sym_filepath, QUVI_SCRIPT_PROPERTY_FILEPATH);
            rb_hash_aset(script, qv_sym_type, type_names[i]);
            quvi_script_get(handle->q, types[i], QUVI_SCRIPT_PROPERTY_DOMAINS, &s);
            if (strlen(s) > 0) {
                VALUE domains = rb_external_str_new_cstr(s);
                rb_hash_aset(script, qv_sym_domains, rb_str_split(domains, ","));
            }
            set_property(qv_sym_export_format, QUVI_SCRIPT_PROPERTY_EXPORT_FORMAT);
#undef set_property

            rb_yield(script);
        }
    }
    return Qnil;
}

typedef struct qv_media_new_params_st
{
    qv_handle_t *handle;
    char *url;
    quvi_media_t res;
} qv_media_new_params_t;

static void* qv_media_new_nogvl(void *data1)
{
    qv_media_new_params_t *params = data1;
    params->res = quvi_media_new(params->handle->q, params->url);
    return 0;
}

VALUE qv_handle_parse_media(VALUE self, VALUE url)
{
    qv_media_new_params_t params;
    VALUE media, streams, stream;
    double d;
    char *s, *best;

    params.handle = DATA_PTR(self);
    params.url = StringValueCStr(url);

    rb_thread_call_without_gvl(qv_media_new_nogvl, &params, RUBY_UBF_IO, 0);
    if (quvi_ok(params.handle->q) == QUVI_FALSE) {
        qv_raise(params.handle, rb_sprintf("unable create quvi_media_t handle (url=%s)", params.url));
    }
    media = rb_hash_new();
#define set_property_str(item, key, id) \
    s = NULL; \
    quvi_media_get(params.res, id, &s); \
    if (s) { rb_hash_aset(item, key, rb_external_str_new_cstr(s)); }
#define set_property_int(item, key, id) \
    d = -1; \
    quvi_media_get(params.res, id, &d); \
    if (d > 0) { rb_hash_aset(item, key, INT2NUM((int)d)); }
    set_property_str(media, qv_sym_id, QUVI_MEDIA_PROPERTY_ID);
    set_property_str(media, qv_sym_title, QUVI_MEDIA_PROPERTY_TITLE);
    set_property_str(media, qv_sym_thumbnail_url, QUVI_MEDIA_PROPERTY_THUMBNAIL_URL);
    set_property_int(media, qv_sym_start_time_ms, QUVI_MEDIA_PROPERTY_START_TIME_MS);
    set_property_int(media, qv_sym_duration_ms, QUVI_MEDIA_PROPERTY_DURATION_MS);
    streams = rb_hash_aset(media, qv_sym_streams, rb_ary_new());
    quvi_media_stream_choose_best(params.res);
    quvi_media_get(params.res, QUVI_MEDIA_STREAM_PROPERTY_ID, &best);
    quvi_media_stream_reset(params.res);
    while (quvi_media_stream_next(params.res) == QUVI_TRUE) {
        stream = rb_hash_new();
        set_property_str(stream, qv_sym_id, QUVI_MEDIA_STREAM_PROPERTY_ID);
        if (strcmp(best, s) == 0) {
            rb_hash_aset(stream, qv_sym_best, Qtrue);
        }
        set_property_int(stream, qv_sym_video_height, QUVI_MEDIA_STREAM_PROPERTY_VIDEO_HEIGHT);
        set_property_int(stream, qv_sym_video_width, QUVI_MEDIA_STREAM_PROPERTY_VIDEO_WIDTH);
        set_property_str(stream, qv_sym_video_encoding, QUVI_MEDIA_STREAM_PROPERTY_VIDEO_ENCODING);
        set_property_int(stream, qv_sym_video_bitrate_kbit_s, QUVI_MEDIA_STREAM_PROPERTY_VIDEO_BITRATE_KBIT_S);
        set_property_str(stream, qv_sym_audio_encoding, QUVI_MEDIA_STREAM_PROPERTY_AUDIO_ENCODING);
        set_property_int(stream, qv_sym_audio_bitrate_kbit_s, QUVI_MEDIA_STREAM_PROPERTY_AUDIO_BITRATE_KBIT_S);
        set_property_str(stream, qv_sym_container, QUVI_MEDIA_STREAM_PROPERTY_CONTAINER);
        set_property_str(stream, qv_sym_url, QUVI_MEDIA_STREAM_PROPERTY_URL);
        rb_ary_push(streams, stream);
    }

#undef set_property_str
#undef set_property_int
    quvi_media_free(params.res);

    return media;
}

typedef struct qv_playlist_new_params_st
{
    qv_handle_t *handle;
    char *url;
    quvi_playlist_t res;
} qv_playlist_new_params_t;

static void* qv_playlist_new_nogvl(void *data1)
{
    qv_media_new_params_t *params = data1;
    params->res = quvi_playlist_new(params->handle->q, params->url);
    return 0;
}

VALUE qv_handle_parse_playlist(VALUE self, VALUE url)
{
    qv_playlist_new_params_t params;
    VALUE playlist, media, medium;
    char *s;
    double d;

    params.handle = DATA_PTR(self);
    params.url = StringValueCStr(url);
    rb_thread_call_without_gvl(qv_playlist_new_nogvl, &params, RUBY_UBF_IO, 0);
    if (quvi_ok(params.handle->q) == QUVI_FALSE) {
        qv_raise(params.handle, rb_sprintf("unable create quvi_media_t handle (url=%s)", params.url));
    }
    playlist = rb_hash_new();
#define set_property_str(item, key, id) \
    s = NULL; \
    quvi_playlist_get(params.res, id, &s); \
    if (s) { rb_hash_aset(item, key, rb_external_str_new_cstr(s)); }
#define set_property_int(item, key, id) \
    d = -1; \
    quvi_playlist_get(params.res, id, &d); \
    if (d > 0) { rb_hash_aset(item, key, INT2NUM((int)d)); }

    set_property_str(playlist, qv_sym_id, QUVI_PLAYLIST_PROPERTY_ID);
    set_property_str(playlist, qv_sym_title, QUVI_PLAYLIST_PROPERTY_TITLE);
    set_property_str(playlist, qv_sym_thumbnail_url, QUVI_PLAYLIST_PROPERTY_THUMBNAIL_URL);
    media = rb_hash_aset(playlist, qv_sym_media, rb_ary_new());
    while (quvi_playlist_media_next(params.res) == QUVI_TRUE) {
        medium = rb_hash_new();
        set_property_str(medium, qv_sym_url, QUVI_PLAYLIST_MEDIA_PROPERTY_URL);
        set_property_str(medium, qv_sym_title, QUVI_PLAYLIST_MEDIA_PROPERTY_TITLE);
        set_property_int(medium, qv_sym_duration_ms, QUVI_PLAYLIST_MEDIA_PROPERTY_DURATION_MS);
        rb_ary_push(media, medium);
    }
#undef set_property_str
#undef set_property_int

    quvi_playlist_free(params.res);
    return playlist;
}

void init_quvi_handle()
{
    qv_cHandle = rb_define_class_under(qv_mQuvi, "Handle", rb_cObject);
    rb_define_alloc_func(qv_cHandle, qv_handle_alloc);
    rb_define_method(qv_cHandle, "initialize", qv_handle_init, -1);
    rb_define_method(qv_cHandle, "supports?", qv_handle_supports_p, -1);
    rb_define_method(qv_cHandle, "autoproxy=", qv_handle_autoproxy_set, 1);
    rb_define_method(qv_cHandle, "user_agent=", qv_handle_user_agent_set, 1);
    rb_define_method(qv_cHandle, "each_script", qv_handle_each_script, 0);
    rb_define_method(qv_cHandle, "parse_media", qv_handle_parse_media, 1);
    rb_define_method(qv_cHandle, "parse_playlist", qv_handle_parse_playlist, 1);

    qv_DEFAULT_USER_AGENT = rb_const_get(qv_cHandle, rb_intern("DEFAULT_USER_AGENT"));
}

void init_symbols()
{
    qv_sym_any = ID2SYM(rb_intern("any"));
    qv_sym_autoproxy = ID2SYM(rb_intern("autoproxy"));
    qv_sym_media = ID2SYM(rb_intern("media"));
    qv_sym_online = ID2SYM(rb_intern("online"));
    qv_sym_playlist = ID2SYM(rb_intern("playlist"));
    qv_sym_subtitle = ID2SYM(rb_intern("subtitle"));
    qv_sym_type = ID2SYM(rb_intern("type"));
    qv_sym_user_agent = ID2SYM(rb_intern("user_agent"));
    qv_sym_export_format = ID2SYM(rb_intern("export_format"));
    qv_sym_filepath = ID2SYM(rb_intern("filepath"));
    qv_sym_domains = ID2SYM(rb_intern("domains"));
    qv_sym_sha1 = ID2SYM(rb_intern("sha1"));
    qv_sym_title = ID2SYM(rb_intern("title"));
    qv_sym_id = ID2SYM(rb_intern("id"));
    qv_sym_thumbnail_url = ID2SYM(rb_intern("thumbnail_url"));
    qv_sym_start_time_ms = ID2SYM(rb_intern("start_time_ms"));
    qv_sym_duration_ms = ID2SYM(rb_intern("duration_ms"));
    qv_sym_video_height = ID2SYM(rb_intern("video_height"));
    qv_sym_video_width = ID2SYM(rb_intern("video_width"));
    qv_sym_video_encoding = ID2SYM(rb_intern("video_encoding"));
    qv_sym_video_bitrate_kbit_s = ID2SYM(rb_intern("video_bitrate_kbit_s"));
    qv_sym_audio_encoding = ID2SYM(rb_intern("audio_encoding"));
    qv_sym_audio_bitrate_kbit_s = ID2SYM(rb_intern("audio_bitrate_kbit_s"));
    qv_sym_container = ID2SYM(rb_intern("container"));
    qv_sym_url = ID2SYM(rb_intern("url"));
    qv_sym_streams = ID2SYM(rb_intern("streams"));
    qv_sym_best = ID2SYM(rb_intern("best"));
}

void Init_quvi_ext()
{
    VALUE qv_mVersion;

    qv_mQuvi = rb_define_module("Quvi");

    qv_mVersion = rb_define_module_under(qv_mQuvi, "VERSION");
    rb_define_const(qv_mVersion, "QUVI_VERSION", rb_str_freeze(rb_external_str_new_cstr(quvi_version(QUVI_VERSION))));
    rb_define_const(qv_mVersion, "QUVI_CONFIGURATION", rb_str_freeze(rb_external_str_new_cstr(quvi_version(QUVI_VERSION_CONFIGURATION))));
    rb_define_const(qv_mVersion, "QUVI_BUILD_CC_CFLAGS", rb_str_freeze(rb_external_str_new_cstr(quvi_version(QUVI_VERSION_BUILD_CC_CFLAGS))));
    rb_define_const(qv_mVersion, "QUVI_BUILD_TARGET", rb_str_freeze(rb_external_str_new_cstr(quvi_version(QUVI_VERSION_BUILD_TARGET))));
    rb_define_const(qv_mVersion, "QUVI_BUILD_TIME", rb_str_freeze(rb_external_str_new_cstr(quvi_version(QUVI_VERSION_BUILD_TIME))));

    qv_eError = rb_const_get(qv_mQuvi, rb_intern("Error"));

    init_symbols();
    init_quvi_handle();
}
