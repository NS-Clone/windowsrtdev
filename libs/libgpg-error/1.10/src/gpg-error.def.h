/* gpg-error.def.in - Exported symbols
 *   Needs to be processed by CPP.
 */

#include <config.h>

EXPORTS
 gpg_strerror                 @1
 gpg_strerror_r               @2  
 gpg_strsource                @3  
 gpg_err_code_from_errno      @4
 gpg_err_code_to_errno        @5
 gpg_err_init		      @6
 gpg_err_code_from_syserror   @7
 gpg_err_set_errno            @8
#ifdef HAVE_W32CE_SYSTEM
   _gpg_w32ce_get_errno       @9
   _gpg_w32ce_strerror        @10
#endif
#ifdef HAVE_W32_SYSTEM
   _gpg_w32_bindtextdomain     @11
   _gpg_w32_textdomain         @12
   _gpg_w32_gettext            @13
   _gpg_w32_dgettext           @14
   _gpg_w32_dngettext          @15
   _gpg_w32_gettext_localename @16
   _gpg_w32_gettext_use_utf8   @17
#endif
 gpg_err_deinit		      @18
