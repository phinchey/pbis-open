/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * lib/crypto/state.c
 *
 * Copyright (C) 2001 by the Massachusetts Institute of Technology.
 * All rights reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  Furthermore if you modify this software you must label
 * your software as modified software and not distribute it in such a
 * fashion that it might be confused with the original M.I.T. software.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express

 *
 *
 *
 *  * Section 6 (Encryption) of the Kerberos revisions document defines
 * cipher states to be used to chain encryptions and decryptions
 * together.  Examples of cipher states include initialization vectors
 * for CBC encription.    This file contains implementations of
 * krb5_c_init_state and krb5_c_free_state used by clients of the
 * Kerberos crypto library.
 */
#include "k5-int.h"
#include "etypes.h"

/* Most enctypes delegate cipher state handling to the enc provider by using
 * this function as their init_state methods. */
krb5_error_code
krb5int_init_state_enc(const struct krb5_keytypes *ktp,
                       const krb5_keyblock *key, krb5_keyusage keyusage,
                       krb5_data *out_state)
{
    return ktp->enc->init_state(key, keyusage, out_state);
}

void
krb5int_free_state_enc(const struct krb5_keytypes *ktp, krb5_data *state)
{
    (void)ktp->enc->free_state(state);
}

krb5_error_code KRB5_CALLCONV
krb5_c_init_state (krb5_context context, const krb5_keyblock *key,
                   krb5_keyusage keyusage, krb5_data *new_state)
{
    const struct krb5_keytypes *ktp;

    ktp = find_enctype(key->enctype);
    if (ktp == NULL)
        return KRB5_BAD_ENCTYPE;
    return ktp->init_state(ktp, key, keyusage, new_state);
}

krb5_error_code KRB5_CALLCONV
krb5_c_free_state(krb5_context context, const krb5_keyblock *key,
                  krb5_data *state)
{
    const struct krb5_keytypes *ktp;

    ktp = find_enctype(key->enctype);
    if (ktp == NULL)
        return KRB5_BAD_ENCTYPE;
    ktp->free_state(ktp, state);
    return 0;
}
