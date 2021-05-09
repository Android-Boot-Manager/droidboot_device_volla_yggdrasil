LOCAL_DIR := $(GET_LOCAL_DIR)



OBJS += \
    lib/lvgl/src/lv_core/lv_group.o \
    lib/lvgl/src/lv_core/lv_indev.o \
    lib/lvgl/src/lv_core/lv_disp.o \
    lib/lvgl/src/lv_core/lv_obj.o \
    lib/lvgl/src/lv_core/lv_refr.o \
    lib/lvgl/src/lv_core/lv_style.o \
    lib/lvgl/src/lv_hal/lv_hal_disp.o \
    lib/lvgl/src/lv_hal/lv_hal_indev.o \
    lib/lvgl/src/lv_hal/lv_hal_tick.o \
lib/lvgl/src/lv_widgets/lv_arc.o \
lib/lvgl/src/lv_widgets/lv_bar.o \
lib/lvgl/src/lv_widgets/lv_checkbox.o \
lib/lvgl/src/lv_widgets/lv_cpicker.o \
lib/lvgl/src/lv_widgets/lv_dropdown.o \
lib/lvgl/src/lv_widgets/lv_keyboard.o \
lib/lvgl/src/lv_widgets/lv_line.o \
lib/lvgl/src/lv_widgets/lv_msgbox.o \
lib/lvgl/src/lv_widgets/lv_spinner.o \
lib/lvgl/src/lv_widgets/lv_roller.o \
lib/lvgl/src/lv_widgets/lv_table.o \
lib/lvgl/src/lv_widgets/lv_tabview.o \
lib/lvgl/src/lv_widgets/lv_tileview.o \
lib/lvgl/src/lv_widgets/lv_btn.o \
lib/lvgl/src/lv_widgets/lv_calendar.o \
lib/lvgl/src/lv_widgets/lv_chart.o \
lib/lvgl/src/lv_widgets/lv_canvas.o \
lib/lvgl/src/lv_widgets/lv_gauge.o \
lib/lvgl/src/lv_widgets/lv_label.o \
lib/lvgl/src/lv_widgets/lv_list.o \
lib/lvgl/src/lv_widgets/lv_slider.o \
lib/lvgl/src/lv_widgets/lv_textarea.o \
lib/lvgl/src/lv_widgets/lv_spinbox.o \
lib/lvgl/src/lv_widgets/lv_btnmatrix.o \
lib/lvgl/src/lv_widgets/lv_cont.o \
lib/lvgl/src/lv_widgets/lv_img.o \
lib/lvgl/src/lv_widgets/lv_imgbtn.o \
lib/lvgl/src/lv_widgets/lv_led.o \
lib/lvgl/src/lv_widgets/lv_linemeter.o \
lib/lvgl/src/lv_widgets/lv_page.o \
lib/lvgl/src/lv_widgets/lv_switch.o \
lib/lvgl/src/lv_widgets/lv_win.o \
lib/lvgl/src/lv_widgets/lv_objmask.o \
lib/lvgl/src/lv_font/lv_font.o \
lib/lvgl/src/lv_font/lv_font_fmt_txt.o \
lib/lvgl/src/lv_font/lv_font_montserrat_12.o \
lib/lvgl/src/lv_font/lv_font_montserrat_14.o \
lib/lvgl/src/lv_font/lv_font_montserrat_16.o \
lib/lvgl/src/lv_font/lv_font_montserrat_18.o \
lib/lvgl/src/lv_font/lv_font_montserrat_20.o \
lib/lvgl/src/lv_font/lv_font_montserrat_22.o \
lib/lvgl/src/lv_font/lv_font_montserrat_24.o \
lib/lvgl/src/lv_font/lv_font_montserrat_26.o \
lib/lvgl/src/lv_font/lv_font_montserrat_28.o \
lib/lvgl/src/lv_font/lv_font_montserrat_30.o \
lib/lvgl/src/lv_font/lv_font_montserrat_32.o \
lib/lvgl/src/lv_font/lv_font_montserrat_34.o \
lib/lvgl/src/lv_font/lv_font_montserrat_36.o \
lib/lvgl/src/lv_font/lv_font_montserrat_38.o \
lib/lvgl/src/lv_font/lv_font_montserrat_40.o \
lib/lvgl/src/lv_font/lv_font_montserrat_42.o \
lib/lvgl/src/lv_font/lv_font_montserrat_44.o \
lib/lvgl/src/lv_font/lv_font_montserrat_46.o \
lib/lvgl/src/lv_font/lv_font_montserrat_48.o \
lib/lvgl/src/lv_font/lv_font_montserrat_12_subpx.o \
lib/lvgl/src/lv_font/lv_font_montserrat_28_compressed.o \
lib/lvgl/src/lv_font/lv_font_unscii_8.o \
lib/lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.o \
lib/lvgl/src/lv_misc/lv_area.o \
lib/lvgl/src/lv_misc/lv_task.o \
lib/lvgl/src/lv_misc/lv_fs.o \
lib/lvgl/src/lv_misc/lv_anim.o \
lib/lvgl/src/lv_misc/lv_mem.o \
lib/lvgl/src/lv_misc/lv_ll.o \
lib/lvgl/src/lv_misc/lv_color.o \
lib/lvgl/src/lv_misc/lv_txt.o \
lib/lvgl/src/lv_misc/lv_txt_ap.o \
lib/lvgl/src/lv_misc/lv_math.o \
lib/lvgl/src/lv_misc/lv_log.o \
lib/lvgl/src/lv_misc/lv_gc.o \
lib/lvgl/src/lv_misc/lv_utils.o \
lib/lvgl/src/lv_misc/lv_async.o \
lib/lvgl/src/lv_misc/lv_printf.o \
lib/lvgl/src/lv_misc/lv_bidi.o \
lib/lvgl/src/lv_misc/lv_debug.o \
lib/lvgl/src/lv_themes/lv_theme.o \
lib/lvgl/src/lv_themes/lv_theme_material.o \
lib/lvgl/src/lv_themes/lv_theme_mono.o \
lib/lvgl/src/lv_themes/lv_theme_empty.o \
lib/lvgl/src/lv_themes/lv_theme_template.o \
lib/lvgl/src/lv_draw/lv_draw_mask.o \
lib/lvgl/src/lv_draw/lv_draw_blend.o \
lib/lvgl/src/lv_draw/lv_draw_rect.o \
lib/lvgl/src/lv_draw/lv_draw_label.o \
lib/lvgl/src/lv_draw/lv_draw_line.o \
lib/lvgl/src/lv_draw/lv_draw_img.o \
lib/lvgl/src/lv_draw/lv_draw_arc.o \
lib/lvgl/src/lv_draw/lv_draw_triangle.o \
lib/lvgl/src/lv_draw/lv_img_decoder.o \
lib/lvgl/src/lv_draw/lv_img_cache.o \
lib/lvgl/src/lv_draw/lv_img_buf.o \
lib/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.o









include make/module.mk

