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

ID qv_sym_any;
ID qv_sym_autoproxy;
ID qv_sym_media;
ID qv_sym_online;
ID qv_sym_playlist;
ID qv_sym_subtitle;
ID qv_sym_type;
ID qv_sym_user_agent;

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
        qv_raise(handle, "unable create quvi_t handle");
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
    VALUE url;
    QuviSupportsType type;
    QuviSupportsMode mode;
    QuviBoolean res;
} qv_supports_params_t;

static void* qv_handle_supports_p_nogvl(void *data1)
{
    qv_supports_params_t *params = data1;
    params->res = quvi_supports(params->handle->q, RSTRING_PTR(params->url),
                                params->mode, params->type);
    return 0;
}

VALUE qv_handle_supports_p(int argc, VALUE *argv, VALUE self)
{
    VALUE opts;
    qv_supports_params_t params;

    params.handle  = DATA_PTR(self);
    params.type = QUVI_SUPPORTS_TYPE_ANY;
    params.mode = QUVI_SUPPORTS_MODE_OFFLINE;
    params.res = QUVI_FALSE;
    rb_scan_args(argc, argv, "11", &params.url, &opts);
    Check_Type(params.url, T_STRING);
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
                rb_raise(rb_eArgError, "unknown type: %s", StringValuePtr(arg));
            }
        }
    }
    rb_thread_call_without_gvl(qv_handle_supports_p_nogvl, &params, RUBY_UBF_IO, 0);
    if (params.res == QUVI_TRUE) {
        return Qtrue;
    }
    if (quvi_errcode(params.handle->q) != QUVI_ERROR_NO_SUPPORT) {
        qv_raise(params.handle, "unable to check if URL supported");
    }
    return Qfalse;
}

void init_quvi_handle()
{
    qv_cHandle = rb_define_class_under(qv_mQuvi, "Handle", rb_cObject);
    rb_define_alloc_func(qv_cHandle, qv_handle_alloc);
    rb_define_method(qv_cHandle, "initialize", qv_handle_init, -1);
    rb_define_method(qv_cHandle, "supports?", qv_handle_supports_p, -1);
    rb_define_method(qv_cHandle, "autoproxy=", qv_handle_autoproxy_set, 1);
    rb_define_method(qv_cHandle, "user_agent=", qv_handle_user_agent_set, 1);

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
