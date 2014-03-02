/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <quvi.h>
#pragma GCC diagnostic pop

#include <ruby.h>

VALUE qv_mQuvi;

void init_quvi_media()
{
}

void Init_quvi_ext()
{
    VALUE qv_mVersion;

    qv_mQuvi = rb_define_module("Quvi");
    qv_mVersion = rb_define_module_under(qv_mQuvi, "VERSION");
    rb_define_const(qv_mVersion, "QUVI_VERSION", rb_external_str_new_cstr(quvi_version(QUVI_VERSION)));
    rb_define_const(qv_mVersion, "QUVI_CONFIGURATION", rb_external_str_new_cstr(quvi_version(QUVI_VERSION_CONFIGURATION)));
    rb_define_const(qv_mVersion, "QUVI_BUILD_CC_CFLAGS", rb_external_str_new_cstr(quvi_version(QUVI_VERSION_BUILD_CC_CFLAGS)));
    rb_define_const(qv_mVersion, "QUVI_BUILD_TARGET", rb_external_str_new_cstr(quvi_version(QUVI_VERSION_BUILD_TARGET)));
    rb_define_const(qv_mVersion, "QUVI_BUILD_TIME", rb_external_str_new_cstr(quvi_version(QUVI_VERSION_BUILD_TIME)));
    init_quvi_media();
}
