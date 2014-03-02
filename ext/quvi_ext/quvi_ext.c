/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <quvi.h>
#pragma GCC diagnostic pop

#include <ruby.h>

VALUE qv_mQuvi;
VALUE qv_cHandle;
VALUE qv_eError;

ID qv_sym_any;
ID qv_sym_media;
ID qv_sym_online;
ID qv_sym_playlist;
ID qv_sym_subtitle;
ID qv_sym_type;

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
static void qv_raise_at(qv_handle_t *handle, const char *message, const char *file, int line)
{
    VALUE exc, str;
    const char *qv_msg;
    char buf[10];
    int qv_code;

    str = rb_str_buf_new_cstr(message ? message : "");
    qv_msg = quvi_errmsg(handle->q);
    qv_code = quvi_errcode(handle->q);
    if (qv_msg) {
        rb_str_buf_cat_ascii(str, ": \"");
        rb_str_buf_cat_ascii(str, qv_msg);
        rb_str_buf_cat_ascii(str, "\" ");
        snprintf(buf, 10, "\" (%d)", qv_code);
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
    handle->q = quvi_new();
    if (quvi_ok(handle->q) == QUVI_FALSE) {
        qv_raise(handle, "can't create quvi_t handle");
    }
    return self;
}

VALUE qv_handle_supports_p(int argc, VALUE *argv, VALUE self)
{
    qv_handle_t *handle = DATA_PTR(self);
    VALUE url, opts;
    QuviSupportsType type = QUVI_SUPPORTS_TYPE_ANY;
    QuviSupportsMode mode = QUVI_SUPPORTS_MODE_OFFLINE;
    QuviBoolean r;

    rb_scan_args(argc, argv, "11", &url, &opts);
    Check_Type(url, T_STRING);
    if (!NIL_P(opts)) {
        VALUE arg;
        Check_Type(opts, T_HASH);
        arg = rb_hash_lookup2(opts, qv_sym_online, Qundef);
        if (arg != Qundef) {
            mode = RTEST(arg) ? QUVI_SUPPORTS_MODE_ONLINE : QUVI_SUPPORTS_MODE_OFFLINE;
        }
        arg = rb_hash_aref(opts, qv_sym_type);
        if (arg != Qnil) {
            if (arg == qv_sym_media) {
                type = QUVI_SUPPORTS_TYPE_MEDIA;
            } else if (arg == qv_sym_playlist) {
                type = QUVI_SUPPORTS_TYPE_PLAYLIST;
            } else if (arg == qv_sym_subtitle) {
                type = QUVI_SUPPORTS_TYPE_SUBTITLE;
            } else if (arg == qv_sym_any) {
                type = QUVI_SUPPORTS_TYPE_ANY;
            } else {
                rb_raise(rb_eArgError, "unknown type: %s", StringValuePtr(arg));
            }
        }
    }

    r = quvi_supports(handle->q, RSTRING_PTR(url), mode, type);
    return r == QUVI_TRUE ? Qtrue : Qfalse;
}

void init_quvi_handle()
{
    qv_cHandle = rb_define_class_under(qv_mQuvi, "Handle", rb_cObject);
    rb_define_alloc_func(qv_cHandle, qv_handle_alloc);
    rb_define_method(qv_cHandle, "initialize", qv_handle_init, -1);
    rb_define_method(qv_cHandle, "supports?", qv_handle_supports_p, -1);
}

void init_symbols()
{
    qv_sym_any = ID2SYM(rb_intern("any"));
    qv_sym_media = ID2SYM(rb_intern("media"));
    qv_sym_online = ID2SYM(rb_intern("online"));
    qv_sym_playlist = ID2SYM(rb_intern("playlist"));
    qv_sym_subtitle = ID2SYM(rb_intern("subtitle"));
    qv_sym_type = ID2SYM(rb_intern("type"));
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
