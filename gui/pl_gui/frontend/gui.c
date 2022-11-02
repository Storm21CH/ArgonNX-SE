/*
 * Copyright (c) 2018-2022 CTCaer
 * Copyright (c) 2019-2022 Storm21
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <bdk.h>
#include "gui.h"
#include "gui_emummc_tools.h"
#include "gui_tools.h"
#include "gui_info.h"
#include <libs/lvgl/lvgl.h>
#include "../gfx/logos-gui.h"
#include "../config.h"
#include <libs/fatfs/ff.h>

extern hekate_config h_cfg;
extern nyx_config n_cfg;
extern volatile boot_cfg_t *b_cfg;
extern volatile nyx_storage_t *nyx_str;

extern lv_res_t launch_payload(lv_obj_t *list);
extern lv_res_t launch_payload_btn(lv_obj_t* obj);


static bool disp_init_done = false;
static bool do_reload = false;



lv_obj_t *payload_list;
lv_obj_t *autorcm_btn;
lv_obj_t* close_btn;
lv_obj_t* close_firstwin;

lv_img_dsc_t *icon_switch;
lv_img_dsc_t *icon_payload;
lv_img_dsc_t *icon_lakka;

lv_img_dsc_t *hekate_bg;

lv_style_t mbox_darken;
lv_style_t hint_small_style;
lv_style_t hint_small_style_white;
lv_style_t monospace_text;

lv_style_t btn_transp_rel, btn_transp_pr;
lv_style_t tabview_btn_pr, tabview_btn_tgl_pr;

lv_style_t header_style;
lv_style_t win_bg_style;
lv_style_t style_kb_rel;
lv_style_t style_kb_pr;

lv_style_t font20_style;
lv_style_t font20red_style;
lv_style_t font20green_style;
lv_style_t labels_style;
lv_style_t inv_label;

char *text_color;

typedef struct _gui_status_bar_ctx
{
	lv_obj_t* battery_more;
	lv_obj_t* time_date;
	lv_obj_t* akkustand;
	lv_obj_t* akkusym;
	lv_obj_t* battery_more_volt;
	lv_obj_t* temperatur;
} gui_status_bar_ctx;

typedef struct _jc_lv_driver_t
{
	lv_indev_t *indev;
	bool centering_done;
	u16 cx_max;
	u16 cx_min;
	u16 cy_max;
	u16 cy_min;
	s16 pos_x;
	s16 pos_y;
	s16 pos_last_x;
	s16 pos_last_y;
	lv_obj_t *cursor;
	u32 cursor_timeout;
	bool cursor_hidden;
	u32 console_timeout;
} jc_lv_driver_t;

static jc_lv_driver_t jc_drv_ctx;

static gui_status_bar_ctx status_bar;

static void _nyx_disp_init()
{
	display_init_framebuffer_pitch_inv();
	display_init_framebuffer_log();
}

//ATM Tab
const char* lbl_start_part = "";//Button Label Partition EmuMMC starten
const char* lbl_sw_part = "";//Schalter Label Partition EmuMMC
const char* mbox_no_emummc_warnung = "";//Funktion Part/SD Schalter mbox Warnung emummc.ini nicht vorhanden!
const char* mbox_no_emummc_sd_warnung = "";//Funktion Part/SD Schalter mbox Warnung emummc_sd.ini nicht vorhanden!

//Tools Tab
const char* lbl_slider_hell = "";//Slider Label Helligkeit
const char* lbl_sw_sprache = "";//Schalter Label Sprache umschalten
const char* lbl_conf_ini = "";//Button Label config.ini bearbeiten
const char* lbl_win_conf_ini = "";//Header Label Win config.ini bearbeiten
const char* lbl_rtc_time = "";//Button Label RTC Zeit und Datum bearbeiten
const char* lbl_win_rtc_time = "";//Header Label Win RTC Zeit und Datum bearbeiten
const char* lbl_cons_info = "";//Button Label Konsolen Info
const char* lbl_title_conf = "";//Titel Label Konfiguration
const char* lbl_title_info = "";//Titel Label Informationen
const char* lbl_themedel_sx = "";//Button Label Theme löschen SX
const char* lbl_themedel_atm = "";//Button Label Theme löschen ATM

//Sonstige Texte und Warnungen Allgemein
const char* mbox_theme_ok = "";//mbox kein theme
const char* mbox_theme_warnung = "";//mbox theme Warnung
const char* mbox_akku_warnung = "";//mbox Akku Warnung
const char* lbl_no_conf_warnung = "";//Label Warnung Config.ini nicht vorhanden oder fehlerhaft!

//PL Tab
const char* lbl_pl_more = "";//Label Info zu viele Payloads

//Funktionen Texte
const char* mbox_scrsh_sav = "";//Funktion Screenshot speichern
const char* mbox_scrsh_sav_ok = "";//Funktion Screenshot gespeichert
const char* mbox_scrsh_fail = "";//Funktion Screenshot Fehler
const char* mbox_reload = "";//Funktion Reload mbox Text
const char* mbox_info = "";//Funktion Info mbox Text
const char* mbox_theme_no = "";//Funktion Theme delete mbox kein theme
const char* mbox_theme_del = "";//Funktion Theme delete mbox theme gelöscht
const char* mbox_emun_kein = "";//Funktion ctrl_emun.. Kein Emunand installiert
const char* mbox_emun1_schon = "";//Funktion ctrl_emun.. Emunand 1 schon installiert
const char* mbox_emun2_schon = "";//Funktion ctrl_emun.. Emunand 2 schon installiert
const char* mbox_emun3_schon = "";//Funktion ctrl_emun.. Emunand 3 schon installiert
const char* mbox_emun_suche = "";//Funktion ctrl_emun.. und emmc.. Suche Dateien
const char* mbox_emun_boot0_ok = "";//Funktion ctrl_emun.. boot0 gefunden
const char* mbox_emun_boot0_fail = "";//Funktion ctrl_emun.. boot0 nicht gefunden
const char* mbox_emun_boot1_ok = "";//Funktion ctrl_emun.. boot1 gefunden
const char* mbox_emun_boot1_fail = "";//Funktion ctrl_emun.. boot1 nicht gefunden
const char* mbox_emun_full00_ok = "";//Funktion ctrl_emun.. full00 gefunden Verschiebe Emunand!
const char* mbox_emun_full00_fail = "";//Funktion ctrl_emun.. full00 nicht gefunden
const char* mbox_emun_inst_ok = "";//Funktion ctrl_emun.. und emmc.. Emunand und EmuMMC installiert
const char* mbox_emmc_emmc_ok = "";//Funktion ctrl_emun.. emmc gefunden
const char* mbox_emmc_emmc_fail = "";//Funktion ctrl_emun.. emmc nicht gefunden
const char* mbox_emmc_move = "";//Funktion ctrl_emun.. emmc verschieben
const char* mbox_conf_sav_ok = "";//Funktion Config.ini mbox save ok
const char* mbox_conf_not_found = "";//Funktion Config.ini nicht gefunden
const char* mbox_rtc_time_saved = "";//Funktion mbox RTC Zeit und Datum gespeichert
const char* lbl_rtc_time_stunde = "";//Funktion RTC Zeit und Datum Label Stunde
const char* lbl_rtc_time_minute = "";//Funktion RTC Zeit und Datum Label Minute
const char* lbl_rtc_time_tag = "";//Funktion RTC Zeit und Datum Label Tag
const char* lbl_rtc_time_monat = "";//Funktion RTC Zeit und Datum Label Monat
const char* lbl_rtc_time_jahr = "";//Funktion RTC Zeit und Datum Label Jahr
const char* mbox_autolaunch_set_ok = "";//Funktion Autolaunch einstellen, Payload als Autolaunch gesetzt!
const char* mbox_autolaunch_reset_ok = "";//Funktion Autolaunch einstellen, Autolaunch reset erledigt!
const char* mbox_autolaunch_title = "";//Funktion Autolaunch einstellen, Titel mbox Autolaunch


//Filemanager
const char* mbox_fm_folderrename_sav = "";//Funktion Dateimanager mbox Ordnername gespeichert
const char* mbox_fm_folder_schon = "";//Funktion Dateimanager mbox Ordner schon vorhanden ---------- in mehreren Funktionen vorhanden
const char* lbl_fm_win_folderrename = "";//Funktion Dateimanager Header Win Ordner umbenennen
const char* mbox_fm_folderrename_root = "";//Funktion Dateimanager mbox SD:/ kann nicht umbenannt werden
const char* mbox_fm_foldernew_sav = "";//Funktion Dateimanager mbox Ordner erstellt
const char* lbl_fm_win_foldernew = "";//Funktion Dateimanager Header Win Ordner erstellen
const char* mbox_fm_folderdel_notfound = "";//Funktion Dateimanager mbox Ordner nicht gefunden
const char* mbox_fm_folderdel_del = "";//Funktion Dateimanager mbox Ordner gelöscht
const char* mbox_fm_folderdel_wirklich = "";//Funktion Dateimanager mbox Soll Ordner gelöscht werden?
const char* mbox_fm_folderdel_root = "";//Funktion Dateimanager mbox SD:/ kann nicht gelöscht werden
const char* mbox_fm_foldercopy_tiefer = "";//Funktion Dateimanager mbox Ordner kann nicht in tiefer gelegenen Ordner kopiert werden
const char* mbox_fm_foldercopy_title = "";//Funktion Dateimanager mbox Ordner Kopieren Titel
const char* mbox_fm_folder_no_clipb = "";//Funktion Dateimanager mbox Ordner kein Ordner in Zwischenablage ---------- in mehreren Funktionen vorhanden
const char* mbox_fm_folder_pre_clipb = "";//Funktion Dateimanager mbox Ordner Text vor Anzeige Ordner in Zwischenablage ---------- in mehreren Funktionen vorhanden
const char* mbox_fm_foldercopy_root = "";//Funktion Dateimanager mbox SD:/ kann nicht kopiert werden
const char* mbox_fm_foldermove_tiefer = "";//Funktion Dateimanager mbox Ordner kann nicht in tiefer gelegenen Ordner verschoben werden
const char* mbox_fm_foldermove_move = "";//Funktion Dateimanager mbox Ordner verschoben
const char* mbox_fm_foldermove_root = "";//Funktion Dateimanager mbox SD:/ kann nicht verschoben werden
const char* mbox_fm_filerename_sav = "";//Funktion Dateimanager mbox Dateiname gespeichert
const char* mbox_fm_file_schon = "";//Funktion Dateimanager mbox Datei bereits vorhanden --------- in mehreren Funktionen vorhanden
const char* lbl_fm_win_filerename = "";//Funktion Dateimanager Header Win Datei umbenennen
const char* mbox_fm_filemove_move = "";//Funktion Dateimanager mbox Datei verschoben
const char* mbox_fm_file_pre_clipb = "";//Funktion Dateimanager mbox Datei Text vor Anzeige Datei in Zwischenablage --------- in mehreren Funktionen vorhanden
const char* mbox_fm_filecopy_title = "";//Funktion Dateimanager mbox Datei kopieren Titel
const char* mbox_fm_filedel_notfound = "";//Funktion Dateimanager mbox Datei nicht gefunden
const char* mbox_fm_filedel_del = "";//Funktion Dateimanager mbox Datei gelöscht
const char* mbox_fm_filedel_wirklich = "";//Funktion Dateimanager mbox Soll Datei gelöscht werden?
const char* mbox_fm_fileoptions_title = "";//Funktion Dateimanager mbox Datei Optionen Titel
const char* mbox_fm_filecopy_copy_ok = "";//Funktion Dateimanager mbox Datei kopieren fertig
const char* mbox_fm_foldercopy_copy_ok = "";//Funktion Dateimanager mbox Ordner kopieren fertig

//Global definierte Texte für andere Libs verfügbar - gui_info.c HW Info Page - gui_tool.c UMS SD mbox Meldungen
const char* lbl_win_hwpage = "";//Header Label Win Hardware Info - benötigt noch Eintrag - extern const char* lbl_win_hwpage; - in gui.h
const char* lbl_txt2_hwpage = "";//label_txt2 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_txt2_hwpage_alt = "";//label_txt2 alternative Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_txt4_hwpage = "";//label_txt4 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_txt3_hwpage = "";//label_txt3 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_txt5_hwpage = "";//label_txt5 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_txt_akku_hwpage = "";//label Akku Text und Symbol Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_txt6_hwpage = "";//label_txt6 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_tip1_umssd = "";//label lbl_tip1 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_tip2_umssd = "";//label lbl_tip2 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_tip3_umssd = "";//label lbl_tip3 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
const char* lbl_lockpick_hwpage;//label Lockpick Info mbox Lockpick

//Texte Farben einstellen
const char* lbl_colorchange = "";//Button Label color.ini bearbeiten Farben einstellen
const char* lbl_win_colorchange = "";//Header Label Win Farben einstellen
const char* mbox_colorchange_color_save = "";//Funktion Farben einstellen mbox color.ini gespeichert
const char* mbox_colorchange_colorini_notfound = "";//Funktion Farben einstellen mbox color.ini nicht vorhanden
const char* lbl_label_colorchange = "";//Funktion Farben einstellen Label Label Farben einstellen
const char* lbl_tab_label_colorchange = "";//Funktion Farben einstellen Label Tab Label Farben einstellen
const char* lbl_pl_icon_colorchange = "";//Funktion Farben einstellen Label Payload Icon Farben einstellen
const char* lbl_label_colorchange_test = "";//Funktion Farben einstellen Label Label Farben einstellen, Beispiel Farbe
const char* lbl_tab_label_colorchange_test = "";//Funktion Farben einstellen Label Tab Label Farben einstellen, Beispiel Farbe
const char* lbl_pl_icon_colorchange_test = "";//Funktion Farben einstellen Label Payload Icon Farben einstellen, Beispiel Farbe


void gui_load_lang_ger()
{
	//ATM Tab
	lbl_start_part = "Partition EmuMMC starten\n";//Button Label Partition EmuMMC starten
	lbl_sw_part = "Partition / SD Files EmuMMC Schalter";//Schalter Label Partition EmuMMC
	mbox_no_emummc_warnung = "emummc.ini nicht vorhanden!";//Funktion Part/SD Schalter mbox Warnung emummc.ini nicht vorhanden!
	mbox_no_emummc_sd_warnung = "emummc_sd.ini nicht vorhanden!\nBitte in Ordner kopieren:\nSD/emunandswitcher/folders atm/emummc/emummc_sd.ini";//Funktion Part/SD Schalter mbox Warnung emummc_sd.ini nicht vorhanden!

	//Tools Tab
	lbl_slider_hell = "Display Helligkeit";//Slider Label Helligkeit
	lbl_sw_sprache = "Sprache umschalten";//Schalter Label Sprache umschalten
	lbl_conf_ini = "Config.ini bearbeiten";//Button Label config.ini bearbeiten
	lbl_win_conf_ini = "Config.ini bearbeiten";//Header Label Win config.ini bearbeiten
	lbl_rtc_time = "RTC Zeit\nund Datum";//Button Label RTC Zeit und Datum bearbeiten
	lbl_win_rtc_time = "RTC Zeit und Datum";//Header Label Win RTC Zeit und Datum bearbeiten
	lbl_cons_info = "Konsolen\nInfo";//Button Label Konsolen Info
	lbl_title_conf = SYMBOL_SETTINGS" Konfigurations Tools";//Titel Label Konfiguration
	lbl_title_info = SYMBOL_INFO" Informationen";//Titel Label Informationen
	lbl_themedel_sx = "Theme SXOS entfernen";//Button Label Theme löschen SX
	lbl_themedel_atm = "Theme ATM entfernen";//Button Label Theme löschen ATM

	//Sonstige Texte und Warnungen Allgemein
	mbox_theme_ok = "OK kein Theme installiert!";//mbox kein theme
	mbox_theme_warnung = "Achtung Theme installiert!\nNach einem Systemsoftware Update\neines Emunandes, muss das Theme\nentfernt werden um den Starterror\nzu vermeiden...";//mbox theme Warnung
	mbox_akku_warnung = SYMBOL_BATTERY_EMPTY"\nWarnung, Akku fast leer! Bitte Ladegerät anschliessen!";//mbox Akku Warnung
	lbl_no_conf_warnung = "Config.ini nicht vorhanden\noder fehlerhaft!";//Label Warnung Config.ini nicht vorhanden oder fehlerhaft!

	//PL Tab
	lbl_pl_more = "Achtung: Mehr als 16 Payloads gefunden! Es werden maximal 16 Payloads angezeigt!";//Label Info zu viele Payloads

	//Funktionen Texte
	mbox_scrsh_sav = SYMBOL_CAMERA"  #FFDD00 Speichere Screenshot...#";//Funktion Screenshot speichern
	mbox_scrsh_sav_ok = SYMBOL_CAMERA"  #96FF00 Screenshot gespeichert!#";//Funktion Screenshot gespeichert
	mbox_scrsh_fail = SYMBOL_WARNING"  #FFDD00 Screenshot fehlgeschlagen!#";//Funktion Screenshot Fehler
	mbox_reload = "#FF8000 Soll die Anwendung#\n#FF8000 neu geladen werden?#";//Funktion Reload mbox Text
	mbox_info = "ArgonNX-SE Version 1.8 by Storm 2022\nerstellt mit Visual Studio und LittlevGL\nbasiert teilweise auf ArgonNX und Hekate\nIcon templates von mrdude\nDanke an alle Programmierer!\nHekate bdk und Libs Version 5.7.2";//Funktion Info mbox Text
	mbox_theme_no = "Kein Theme installiert...";//Funktion Theme delete mbox kein theme
	mbox_theme_del = "Theme Ordner entfernt...";//Funktion Theme delete mbox theme gelöscht
	mbox_emun_kein = "Kein Emunand installiert!";//Funktion ctrl_emun.. Kein Emunand installiert
	mbox_emun1_schon = "Emunand 1 schon installiert!";//Funktion ctrl_emun.. Emunand 1 schon installiert
	mbox_emun2_schon = "Emunand 2 schon installiert!";//Funktion ctrl_emun.. Emunand 2 schon installiert
	mbox_emun3_schon = "Emunand 3 schon installiert!";//Funktion ctrl_emun.. Emunand 3 schon installiert
	mbox_emun_suche = "Suche Dateien!";//Funktion ctrl_emun.. und emmc.. Suche Dateien
	mbox_emun_boot0_ok = "boot0.bin vorhanden!";//Funktion ctrl_emun.. boot0 gefunden
	mbox_emun_boot0_fail = "boot0.bin nicht vorhanden!";//Funktion ctrl_emun.. boot0 nicht gefunden
	mbox_emun_boot1_ok = "boot1.bin vorhanden!";//Funktion ctrl_emun.. boot1 gefunden
	mbox_emun_boot1_fail = "boot1.bin nicht vorhanden!";//Funktion ctrl_emun.. boot1 nicht gefunden
	mbox_emun_full00_ok = "full.00.bin vorhanden! Verschiebe Emunand!";//Funktion ctrl_emun.. full00 gefunden Verschiebe Emunand!
	mbox_emun_full00_fail = "full.00.bin nicht vorhanden!";//Funktion ctrl_emun.. full00 nicht gefunden
	mbox_emun_inst_ok = "Emunand erfolgreich verschoben...";//Funktion ctrl_emun.. und emmc.. Emunand und EmuMMC installiert
	mbox_emmc_emmc_ok = "eMMC vorhanden!";//Funktion ctrl_emun.. emmc gefunden
	mbox_emmc_emmc_fail = "eMMC nicht vorhanden!";//Funktion ctrl_emun.. emmc nicht gefunden
	mbox_emmc_move = "Verschiebe Emunand!";//Funktion ctrl_emun.. emmc verschieben
	mbox_conf_sav_ok = "Config.ini gespeichert!";//Funktion Config.ini mbox save ok
	mbox_conf_not_found = "Config.ini nicht gefunden!\nBitte in Ordner emunandswitcher kopieren...";//Funktion Config.ini nicht gefunden
	mbox_rtc_time_saved = "RTC Zeit und Datum gespeichert!";//Funktion RTC Zeit und Datum gespeichert
	lbl_rtc_time_stunde = "Stunde";//Funktion RTC Zeit und Datum Label Stunde
	lbl_rtc_time_minute = "Minute";//Funktion RTC Zeit und Datum Label Minute
	lbl_rtc_time_tag = "Tag";//Funktion RTC Zeit und Datum Label Tag
	lbl_rtc_time_monat = "Monat";//Funktion RTC Zeit und Datum Label Monat
	lbl_rtc_time_jahr = "Jahr";//Funktion RTC Zeit und Datum Label Jahr
	mbox_autolaunch_set_ok = "Payload als Autolaunch gesetzt!";//Funktion Autolaunch einstellen, Payload als Autolaunch gesetzt!
	mbox_autolaunch_reset_ok = "Autolaunch Payload entfernt!";//Funktion Autolaunch einstellen, Autolaunch reset erledigt!
	mbox_autolaunch_title = "#FF8000 Welcher Payload soll als Autolaunch#\n#FF8000 eingerichtet werden?#";//Funktion Autolaunch einstellen, Titel mbox Autolaunch

	//Filemanager
	mbox_fm_folderrename_sav = "Ordnername gespeichert!";//Funktion Dateimanager mbox Ordnername gespeichert
	mbox_fm_folder_schon = "Ordner schon vorhanden!";//Funktion Dateimanager mbox Ordner schon vorhanden ---------- in mehreren Funktionen vorhanden
	lbl_fm_win_folderrename = "Ordner umbenennen";//Funktion Dateimanager Header Win Ordner umbenennen
	mbox_fm_folderrename_root = "SD:/ kann nicht umbenannt werden!";//Funktion Dateimanager mbox SD:/ kann nicht umbenannt werden
	mbox_fm_foldernew_sav = "Ordner erstellt!";//Funktion Dateimanager mbox Ordner erstellt
	lbl_fm_win_foldernew = "Ordner erstellen";//Funktion Dateimanager Header Win Ordner erstellen
	mbox_fm_folderdel_notfound = "Ordner nicht gefunden!";//Funktion Dateimanager mbox Ordner nicht gefunden
	mbox_fm_folderdel_del = "Ordner entfernt!";//Funktion Dateimanager mbox Ordner gelöscht
	mbox_fm_folderdel_wirklich = "\n#FF8000 Ordner wirklich entfernen?#";//Funktion Dateimanager mbox Soll Ordner gelöscht werden?
	mbox_fm_folderdel_root = "SD:/ kann nicht entfernt werden!";//Funktion Dateimanager mbox SD:/ kann nicht gelöscht werden
	mbox_fm_foldercopy_tiefer = "Ordner kann nicht in tiefer gelegenen Ordner kopiert werden!";//Funktion Dateimanager mbox Ordner kann nicht in tiefer gelegenen Ordner kopiert werden
	mbox_fm_foldercopy_title = "\n#FF8000 Kopiere Ordner...#\n";//Funktion Dateimanager mbox Ordner Kopieren Titel
	mbox_fm_folder_no_clipb = "Kein Ordner in Zwischenablage!";//Funktion Dateimanager mbox Ordner kein Ordner in Zwischenablage ---------- in mehreren Funktionen vorhanden
	mbox_fm_folder_pre_clipb = "Ordner Clipboard: ";//Funktion Dateimanager mbox Ordner Text vor Anzeige Ordner in Zwischenablage ---------- in mehreren Funktionen vorhanden
	mbox_fm_foldercopy_root = "SD:/ kann nicht kopiert werden!";//Funktion Dateimanager mbox SD:/ kann nicht kopiert werden
	mbox_fm_foldermove_tiefer = "Ordner kann nicht in tiefer gelegenen Ordner verschoben werden!";//Funktion Dateimanager mbox Ordner kann nicht in tiefer gelegenen Ordner verschoben werden
	mbox_fm_foldermove_move = "Ordner verschoben!";//Funktion Dateimanager mbox Ordner verschoben
	mbox_fm_foldermove_root = "SD:/ kann nicht verschoben werden!";//Funktion Dateimanager mbox SD:/ kann nicht verschoben werden
	mbox_fm_filerename_sav = "Dateiname gespeichert!";//Funktion Dateimanager mbox Dateiname gespeichert
	mbox_fm_file_schon = "Datei schon vorhanden!";//Funktion Dateimanager mbox Datei bereits vorhanden --------- in mehreren Funktionen vorhanden
	lbl_fm_win_filerename = "Datei umbenennen";//Funktion Dateimanager Header Win Datei umbenennen
	mbox_fm_filemove_move = "Datei verschoben!";//Funktion Dateimanager mbox Datei verschoben
	mbox_fm_file_pre_clipb = "Datei Clipboard: ";//Funktion Dateimanager mbox Datei Text vor Anzeige Datei in Zwischenablage --------- in mehreren Funktionen vorhanden
	mbox_fm_filecopy_title = "\n#FF8000 Kopiere Datei...#\n";//Funktion Dateimanager mbox Datei kopieren Titel
	mbox_fm_filedel_notfound = "Datei nicht gefunden!";//Funktion Dateimanager mbox Datei nicht gefunden
	mbox_fm_filedel_del = "Datei entfernt!";//Funktion Dateimanager mbox Datei gelöscht
	mbox_fm_filedel_wirklich = "\n#FF8000 Datei wirklich entfernen?#";//Funktion Dateimanager mbox Soll Datei gelöscht werden?
	mbox_fm_fileoptions_title = "\n#FF8000 Datei Optionen#";//Funktion Dateimanager mbox Datei Optionen Titel
	mbox_fm_filecopy_copy_ok = "Datei kopiert!";//Funktion Dateimanager mbox Datei kopieren fertig
	mbox_fm_foldercopy_copy_ok = "Ordner Kopiert!";//Funktion Dateimanager mbox Ordner kopieren fertig

	//Global definierte Texte für andere Libs verfügbar - gui_info.c HW Info Page - gui_tool.c UMS SD mbox Meldungen
	lbl_win_hwpage = "Konsolen Infos und Dumps erstellen";//Header Label Win Hardware Info - benötigt noch Eintrag - extern const char* lbl_win_hwpage; - in gui.h
	lbl_txt2_hwpage = "Sehe Ipatches an und dumpe die nicht gepatchte und gepatchte\nVersionen des BootROMs.\n"
		"Oder dumpe alle Keys der Konsole via #C7EA46 Lockpick RCM#.\n";//label_txt2 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt2_hwpage_alt = "Sehe Ipatches an und dumpe die nicht gepatchte und gepatchte\nVersionen des BootROMs. Oder dumpe Keys via #C7EA46 Lockpick RCM#.\n"
		"#FFDD00 argon/payloads/Lockpick.bin nicht vorhanden oder alt!#\n";//label_txt2 alternative Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt4_hwpage = "Betrachte und dumpe die gecachten #C7EA46 Fuses# und #C7EA46 KFuses#.\n"
		"Fuses Informationen zum SoC und Konsole und KFuses mit HDCP.\n"
		"Es gibt auch Infos von #C7EA46 DRAM#, #C7EA46 Screen# und #C7EA46 Touch panel#.";//label_txt4 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt3_hwpage = "Speicher & Akku Infos";//label_txt3 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt5_hwpage = "Informationen zum eMMC oder microSD und den Partitionen.\n"
		"Es kann auch die Lesegeschwindigkeit gemessen werden.";//label_txt5 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt_akku_hwpage = SYMBOL_BATTERY_FULL"  Akku";//label Akku Text und Symbol Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt6_hwpage = "Informationen zum Akku und Akkulader anzeigen.\n"
		"Auch das Register vom Akku laden kann gedumpt werden.\n";//label_txt6 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_tip1_umssd = "Info: Um zu beenden, #C7EA46 Hardware sicher entfernen# im System machen.\n"
		"                           #FFDD00 Kabel nicht entfernen!#";//label lbl_tip1 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_tip2_umssd = "    Info: Um zu beenden, #C7EA46 Hardware sicher entfernen# im System machen.\n"
		"#FFDD00 Wenn es nicht gemountet ist, muss möglicherweise das Kabel entfernt werden!#";//label lbl_tip2 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_tip3_umssd = "Info: Um zu beenden, #C7EA46 Hardware sicher entfernen# im System machen.\n"
		"                                   oder das Kabel entfernen!#";//label lbl_tip3 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_lockpick_hwpage = "#FF8000 Lockpick RCM#\n\n#FFFFFF Hier wird Lockpick RCM geladen.#\n#FFFFFF Willst du fortfahren?#\n\n";//Lockpick Info mbox Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h

	//Texte Farben einstellen
	lbl_colorchange = "Menu\nFarben";//Button Label color.ini bearbeiten Farben einstellen
	lbl_win_colorchange = "Menu Farben einstellen";//Header Label Win Farben einstellen
	mbox_colorchange_color_save = "Farben in color.ini gespeichert!";//Funktion Farben einstellen mbox color.ini gespeichert
	mbox_colorchange_colorini_notfound = "Color.ini nicht gefunden! Bitte benutze die Farbeinstellungen um eine color.ini zu erzeugen...";//Funktion Farben einstellen mbox color.ini nicht vorhanden
	lbl_label_colorchange = "Hier kannst du die Textfarbe einstellen.";//Funktion Farben einstellen Label Label Farben einstellen
	lbl_tab_label_colorchange = "Hier kannst du die Textfarbe der Tabs einstellen.";//Funktion Farben einstellen Label Tab Label Farben einstellen
	lbl_pl_icon_colorchange = "Hier kannst du die Farbe des standart Payload Icons einstellen.";//Funktion Farben einstellen Label Payload Icon Farben einstellen
	lbl_label_colorchange_test = "Beispiel: Ich bin die eingestellte Text Farbe...";//Funktion Farben einstellen Label Label Farben einstellen, Beispiel Farbe
	lbl_tab_label_colorchange_test = "Beispiel: Ich bin die eingestellte Tab Text Farbe...";//Funktion Farben einstellen Label Tab Label Farben einstellen, Beispiel Farbe
	lbl_pl_icon_colorchange_test = "Beispiel: Ich bin die eingestellte Payload Symbol Farbe...";//Funktion Farben einstellen Label Payload Icon Farben einstellen, Beispiel Farbe
}

void gui_load_lang_eng()
{
	//ATM Tab
	lbl_start_part = "Start Partition EmuMMC\n";//Button Label Partition EmuMMC starten
	lbl_sw_part = "Partition / SD Files EmuMMC Switch";//Schalter Label Partition EmuMMC
	mbox_no_emummc_warnung = "emummc.ini not available!";//Funktion Part/SD Schalter mbox Warnung emummc.ini nicht vorhanden!
	mbox_no_emummc_sd_warnung = "emummc_sd.ini not available!\nPlease copy to folder:\nSD/emunandswitcher/folders atm/emummc/emummc_sd.ini";//Funktion Part/SD Schalter mbox Warnung emummc_sd.ini nicht vorhanden!

	//Tools Tab
	lbl_slider_hell = "Display brightness";//Slider Label Helligkeit
	lbl_sw_sprache = "  Choose language";//Schalter Label Sprache umschalten
	lbl_conf_ini = "Edit Config.ini";//Button Label config.ini bearbeiten
	lbl_win_conf_ini = "Edit Config.ini";//Header Label Win config.ini bearbeiten
	lbl_rtc_time = "RTC Time\nand Date";//Button Label RTC Zeit und Datum bearbeiten
	lbl_win_rtc_time = "RTC Time and Date";//Header Label Win RTC Zeit und Datum bearbeiten
	lbl_cons_info = "Hardware\nInfo";//Button Label Konsolen Info
	lbl_title_conf = SYMBOL_SETTINGS" Configuration Tools";//Titel Label Konfiguration
	lbl_title_info = SYMBOL_INFO" Information";//Titel Label Informationen
	lbl_themedel_sx = "Delete Theme SXOS";//Button Label Theme löschen SX
	lbl_themedel_atm = "Delete Theme ATM";//Button Label Theme löschen ATM

	//Sonstige Texte und Warnungen Allgemein
	mbox_theme_ok = "OK no Theme installed!";//mbox kein theme
	mbox_theme_warnung = "Attention Theme installed!\nAfter a system software update\nof an emunand, the theme must\nbe removed to avoid the\nstartup error...";//mbox theme Warnung
	mbox_akku_warnung = SYMBOL_BATTERY_EMPTY"\nWarning, battery almost empty! Please connect the charger!";//mbox Akku Warnung
	lbl_no_conf_warnung = "Config.ini not available\nor faulty!";//Label Warnung Config.ini nicht vorhanden oder fehlerhaft!

	//PL Tab
	lbl_pl_more = "Attention: More than 16 payloads found! A maximum of 16 payloads are displayed!";//Label Info zu viele Payloads

	//Funktionen Texte
	mbox_scrsh_sav = SYMBOL_CAMERA"  #FFDD00 Saving screenshot...#";//Funktion Screenshot speichern
	mbox_scrsh_sav_ok = SYMBOL_CAMERA"  #96FF00 Screenshot saved!#";//Funktion Screenshot gespeichert
	mbox_scrsh_fail = SYMBOL_WARNING"  #FFDD00 Screenshot failed!#";//Funktion Screenshot Fehler
	mbox_reload = "#FF8000 Do you really want#\n#FF8000 to reboot the app?#";//Funktion Reload mbox Text
	mbox_info = "ArgonNX-SE Version 1.8 by Storm 2022\ncreated with Visual Studio and LittlevGL\npartially based on ArgonNX und Hekate\nIcon templates from mrdude\nThanks to the programmers!\nHekate bdk and Libs Version 5.7.2";//Funktion Info mbox Text
	mbox_theme_no = "No Theme installed...";//Funktion Theme delete mbox kein theme
	mbox_theme_del = "Theme Folder deleted...";//Funktion Theme delete mbox theme gelöscht
	mbox_emun_kein = "No Emunand installed!";//Funktion ctrl_emun.. Kein Emunand installiert
	mbox_emun1_schon = "Emunand 1 already installed!";//Funktion ctrl_emun.. Emunand 1 schon installiert
	mbox_emun2_schon = "Emunand 2 already installed!";//Funktion ctrl_emun.. Emunand 2 schon installiert
	mbox_emun3_schon = "Emunand 3 already installed!";//Funktion ctrl_emun.. Emunand 3 schon installiert
	mbox_emun_suche = "Searching files!";//Funktion ctrl_emun.. und emmc.. Suche Dateien
	mbox_emun_boot0_ok = "boot0.bin available!";//Funktion ctrl_emun.. boot0 gefunden
	mbox_emun_boot0_fail = "boot0.bin not available!";//Funktion ctrl_emun.. boot0 nicht gefunden
	mbox_emun_boot1_ok = "boot1.bin available!";//Funktion ctrl_emun.. boot1 gefunden
	mbox_emun_boot1_fail = "boot1.bin not available!";//Funktion ctrl_emun.. boot1 nicht gefunden
	mbox_emun_full00_ok = "full.00.bin available! Move Emunand!";//Funktion ctrl_emun.. full00 gefunden Verschiebe Emunand!
	mbox_emun_full00_fail = "full.00.bin not available!";//Funktion ctrl_emun.. full00 nicht gefunden
	mbox_emun_inst_ok = "Emunand successfully moved...";////Funktion ctrl_emun.. und emmc.. Emunand und EmuMMC installiert
	mbox_emmc_emmc_ok = "eMMC available!";//Funktion ctrl_emun.. emmc gefunden
	mbox_emmc_emmc_fail = "eMMC not available!";//Funktion ctrl_emun.. emmc nicht gefunden
	mbox_emmc_move = "Move Emunand!";//Funktion ctrl_emun.. emmc verschieben
	mbox_conf_sav_ok = "Config.ini saved!";//Funktion Config.ini mbox save ok
	mbox_conf_not_found = "Config.ini not found!\nPlease copy to folder emunandswitcher...";//Funktion Config.ini nicht gefunden
	mbox_rtc_time_saved = "RTC Time and Date saved!";//Funktion RTC Zeit und Datum gespeichert
	lbl_rtc_time_stunde = "Hour";//Funktion RTC Zeit und Datum Label Stunde
	lbl_rtc_time_minute = "Minute";//Funktion RTC Zeit und Datum Label Minute
	lbl_rtc_time_tag = "Day";//Funktion RTC Zeit und Datum Label Tag
	lbl_rtc_time_monat = "Month";//Funktion RTC Zeit und Datum Label Monat
	lbl_rtc_time_jahr = "Year";//Funktion RTC Zeit und Datum Label Jahr
	mbox_autolaunch_set_ok = "Payload set as Autolaunch!";//Funktion Autolaunch einstellen, Payload als Autolaunch gesetzt!
	mbox_autolaunch_reset_ok = "Autolaunch payload removed!";//Funktion Autolaunch einstellen, Autolaunch reset erledigt!
	mbox_autolaunch_title = "#FF8000 Which payload should be set up#\n#FF8000 as autolaunch?#";//Funktion Autolaunch einstellen, Titel mbox Autolaunch

	//Filemanager
	mbox_fm_folderrename_sav = "Foldername saved!";//Funktion Dateimanager mbox Ordnername gespeichert
	mbox_fm_folder_schon = "Folder already available!";//Funktion Dateimanager mbox Ordner schon vorhanden ---------- in mehreren Funktionen vorhanden
	lbl_fm_win_folderrename = "Folder rename";//Funktion Dateimanager Header Win Ordner umbenennen
	mbox_fm_folderrename_root = "SD:/ cannot be renamed!";//Funktion Dateimanager mbox SD:/ kann nicht umbenannt werden
	mbox_fm_foldernew_sav = "Folder created!";//Funktion Dateimanager mbox Ordner erstellt
	lbl_fm_win_foldernew = "Create folder";//Funktion Dateimanager Header Win Ordner erstellen
	mbox_fm_folderdel_notfound = "Folder not found!";//Funktion Dateimanager mbox Ordner nicht gefunden
	mbox_fm_folderdel_del = "Folder deleted!";//Funktion Dateimanager mbox Ordner gelöscht
	mbox_fm_folderdel_wirklich = "\n#FF8000 Really remove folder?#";//Funktion Dateimanager mbox Soll Ordner gelöscht werden?
	mbox_fm_folderdel_root = "SD:/ cannot be removed!";//Funktion Dateimanager mbox SD:/ kann nicht gelöscht werden
	mbox_fm_foldercopy_tiefer = "Folder cannot be copied to a lower folder!";//Funktion Dateimanager mbox Ordner kann nicht in tiefer gelegenen Ordner kopiert werden
	mbox_fm_foldercopy_title = "\n#FF8000 Copy folder...#\n";//Funktion Dateimanager mbox Ordner Kopieren Titel
	mbox_fm_folder_no_clipb = "No folder on the clipboard!";//Funktion Dateimanager mbox Ordner kein Ordner in Zwischenablage ---------- in mehreren Funktionen vorhanden
	mbox_fm_folder_pre_clipb = "Folder Clipboard: ";//Funktion Dateimanager mbox Ordner Text vor Anzeige Ordner in Zwischenablage ---------- in mehreren Funktionen vorhanden
	mbox_fm_foldercopy_root = "SD:/ cannot be copied!";//Funktion Dateimanager mbox SD:/ kann nicht kopiert werden
	mbox_fm_foldermove_tiefer = "Folder cannot be moved to lower folders!";//Funktion Dateimanager mbox Ordner kann nicht in tiefer gelegenen Ordner verschoben werden
	mbox_fm_foldermove_move = "Folder moved!";//Funktion Dateimanager mbox Ordner verschoben
	mbox_fm_foldermove_root = "SD:/ cannot be moved!";//Funktion Dateimanager mbox SD:/ kann nicht verschoben werden
	mbox_fm_filerename_sav = "Filename saved!";//Funktion Dateimanager mbox Dateiname gespeichert
	mbox_fm_file_schon = "File already exists!";//Funktion Dateimanager mbox Datei bereits vorhanden --------- in mehreren Funktionen vorhanden
	lbl_fm_win_filerename = "File rename";//Funktion Dateimanager Header Win Datei umbenennen
	mbox_fm_filemove_move = "File moved!";//Funktion Dateimanager mbox Datei verschoben
	mbox_fm_file_pre_clipb = "File Clipboard: ";//Funktion Dateimanager mbox Datei Text vor Anzeige Datei in Zwischenablage --------- in mehreren Funktionen vorhanden
	mbox_fm_filecopy_title = "\n#FF8000 Copy file...#\n";//Funktion Dateimanager mbox Datei kopieren Titel
	mbox_fm_filedel_notfound = "File not found!";//Funktion Dateimanager mbox Datei nicht gefunden
	mbox_fm_filedel_del = "File deleted!";//Funktion Dateimanager mbox Datei gelöscht
	mbox_fm_filedel_wirklich = "\n#FF8000 Really remove the file?#";//Funktion Dateimanager mbox Soll Datei gelöscht werden?
	mbox_fm_fileoptions_title = "\n#FF8000 File options#";//Funktion Dateimanager mbox Datei Optionen Titel
	mbox_fm_filecopy_copy_ok = "File copied!";//Funktion Dateimanager mbox Datei kopieren fertig
	mbox_fm_foldercopy_copy_ok = "Folder copied!";//Funktion Dateimanager mbox Ordner kopieren fertig

	//Global definierte Texte für andere Libs verfügbar - gui_info.c HW Info Page - gui_tool.c UMS SD mbox Meldungen
	lbl_win_hwpage = "Hardware infos and dumps";//Header Label Win Hardware Info - benötigt noch Eintrag - extern const char* lbl_win_hwpage; - in gui.h
	lbl_txt2_hwpage = "View Ipatches and dump the unpatched and patched versions\nof BootROM.\n"
		"Or dump every single key via #C7EA46 Lockpick RCM#.\n";//label_txt2 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt2_hwpage_alt = "View Ipatches and dump the unpatched and patched versions\nof BootROM. Or dump every single key via #C7EA46 Lockpick RCM#.\n"
		"#FFDD00 argon/payloads/Lockpick.bin is missing or old!#\n";//label_txt2 alternative Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt4_hwpage = "View and dump the cached #C7EA46 Fuses# and #C7EA46 KFuses#.\n"
		"Fuses contain info about the SoC/SKU and KFuses HDCP keys.\n"
		"You can also see info about #C7EA46 DRAM#, #C7EA46 Screen# and #C7EA46 Touch panel#.";//label_txt4 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt3_hwpage = "Storage & Battery Info";//label_txt3 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt5_hwpage = "View info about the eMMC or microSD and their partition list.\n"
		"Additionally you can benchmark read speeds.";//label_txt5 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt_akku_hwpage = SYMBOL_BATTERY_FULL"  Battery";//label Akku Text und Symbol Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_txt6_hwpage = "View battery and battery charger related info.\n"
		"Additionally you can dump battery charger's registers.\n";//label_txt6 Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_tip1_umssd = "Note: To end it, #C7EA46 safely eject# from inside the OS.\n"
		"       #FFDD00 DO NOT remove the cable!#";//label lbl_tip1 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_tip2_umssd = "Note: To end it, #C7EA46 safely eject# from inside the OS.\n"
		"       #FFDD00 If it's not mounted, you might need to remove the cable!#";//label lbl_tip2 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_tip3_umssd = "Note: To end it, #C7EA46 safely eject# from inside the OS\n"
		"       or by removing the cable!#";//label lbl_tip3 mbox UMS SD - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h
	lbl_lockpick_hwpage = "#FF8000 Lockpick RCM#\n\n#FFFFFF This will launch Lockpick RCM.#\n#FFFFFF Do you want to continue?#\n\n";//Lockpick Info mbox Win Hardware Info - benötigt noch Eintrag - extern const char* XXXXXX; - in gui.h

	//Texte Farben einstellen
	lbl_colorchange = "Menu\nColors";//Button Label color.ini bearbeiten Farben einstellen
	lbl_win_colorchange = "Set Menu Colors";//Header Label Win Farben einstellen
	mbox_colorchange_color_save = "Colors saved in color.ini!";//Funktion Farben einstellen mbox color.ini gespeichert
	mbox_colorchange_colorini_notfound = "Color.ini not found! Please use the color settings to create a color.ini...";//Funktion Farben einstellen mbox color.ini nicht vorhanden
	lbl_label_colorchange = "Here you can set the text color.";//Funktion Farben einstellen Label Label Farben einstellen
	lbl_tab_label_colorchange = "Here you can set the text color of the tabs.";//Funktion Farben einstellen Label Tab Label Farben einstellen
	lbl_pl_icon_colorchange = "Here you can set the color of the default payload icon.";//Funktion Farben einstellen Label Payload Icon Farben einstellen
	lbl_label_colorchange_test = "Example: I am the set text color...";//Funktion Farben einstellen Label Label Farben einstellen, Beispiel Farbe
	lbl_tab_label_colorchange_test = "Example: I am the set tab text color...";//Funktion Farben einstellen Label Tab Label Farben einstellen, Beispiel Farbe
	lbl_pl_icon_colorchange_test = "Example: I am the set payload icon color...";//Funktion Farben einstellen Label Payload Icon Farben einstellen, Beispiel Farbe
}

static void _save_log_to_bmp(char *fname)
{
	u32 *fb_ptr = (u32 *)LOG_FB_ADDRESS;

	// Check if there's log written.
	bool log_changed = false;
	for (u32 i = 0; i < 0xCD000; i++)
	{
		if (fb_ptr[i] != 0)
		{
			log_changed = true;
			break;
		}
	}

	if (!log_changed)
		return;

	const u32 file_size = 0x334000 + 0x36;
	u8 *bitmap = malloc(file_size);

	// Reconstruct FB for bottom-top, landscape bmp.
	u32 *fb = malloc(0x334000);
	for (int x = 1279; x > - 1; x--)
	{
		for (int y = 655; y > -1; y--)
			fb[y * 1280 + x] = *fb_ptr++;
	}

	manual_system_maintenance(true);

	memcpy(bitmap + 0x36, fb, 0x334000);

	typedef struct _bmp_t
	{
		u16 magic;
		u32 size;
		u32 rsvd;
		u32 data_off;
		u32 hdr_size;
		u32 width;
		u32 height;
		u16 planes;
		u16 pxl_bits;
		u32 comp;
		u32 img_size;
		u32 res_h;
		u32 res_v;
		u64 rsvd2;
	} __attribute__((packed)) bmp_t;

	bmp_t *bmp = (bmp_t *)bitmap;

	bmp->magic    = 0x4D42;
	bmp->size     = file_size;
	bmp->rsvd     = 0;
	bmp->data_off = 0x36;
	bmp->hdr_size = 40;
	bmp->width    = 1280;
	bmp->height   = 656;
	bmp->planes   = 1;
	bmp->pxl_bits = 32;
	bmp->comp     = 0;
	bmp->img_size = 0x334000;
	bmp->res_h    = 2834;
	bmp->res_v    = 2834;
	bmp->rsvd2    = 0;

	char path[0x80];
	strcpy(path, "argon/screenshots");
	s_printf(path + strlen(path), "/scr%s_log.bmp", fname);
	sd_save_to_file(bitmap, file_size, path);

	free(bitmap);
	free(fb);
}

static void _save_fb_to_bmp()
{
	// Disallow screenshots if less than 2s passed.
	static u32 timer = 0;
	if (get_tmr_ms() < timer)
		return;

	if (do_reload)
		return;

	const u32 file_size = 0x384000 + 0x36;
	u8 *bitmap = malloc(file_size);
	u32 *fb = malloc(0x384000);
	u32 *fb_ptr = (u32 *)NYX_FB_ADDRESS;

	// Reconstruct FB for bottom-top, landscape bmp.
	for (u32 x = 0; x < 1280; x++)
	{
		for (int y = 719; y > -1; y--)
			fb[y * 1280 + x] = *fb_ptr++;
	}

	static lv_style_t bg;
	lv_style_copy(&bg, &lv_style_pretty);
	bg.body.opa = LV_OPA_0;
	bg.text.color = LV_COLOR_WHITE;

	// Create notification box.
	lv_obj_t * mbox = lv_mbox_create(lv_layer_top(), NULL);
	lv_mbox_set_recolor_text(mbox, true);
	lv_mbox_set_text(mbox, mbox_scrsh_sav);
	lv_obj_set_width(mbox, LV_DPI * 4);
	lv_obj_set_top(mbox, true);
	lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);

	manual_system_maintenance(true);

	memcpy(bitmap + 0x36, fb, 0x384000);

	typedef struct _bmp_t
	{
		u16 magic;
		u32 size;
		u32 rsvd;
		u32 data_off;
		u32 hdr_size;
		u32 width;
		u32 height;
		u16 planes;
		u16 pxl_bits;
		u32 comp;
		u32 img_size;
		u32 res_h;
		u32 res_v;
		u64 rsvd2;
	} __attribute__((packed)) bmp_t;

	bmp_t *bmp = (bmp_t *)bitmap;

	bmp->magic    = 0x4D42;
	bmp->size     = file_size;
	bmp->rsvd     = 0;
	bmp->data_off = 0x36;
	bmp->hdr_size = 40;
	bmp->width    = 1280;
	bmp->height   = 720;
	bmp->planes   = 1;
	bmp->pxl_bits = 32;
	bmp->comp     = 0;
	bmp->img_size = 0x384000;
	bmp->res_h    = 2834;
	bmp->res_v    = 2834;
	bmp->rsvd2    = 0;

	sd_mount();

	char path[0x80];

	strcpy(path, "argon");
	f_mkdir(path);
	strcat(path, "/screenshots");
	f_mkdir(path);

	// Create date/time name.
	char fname[32];
	rtc_time_t time;
	max77620_rtc_get_time(&time);
	if (n_cfg.timeoff)
	{
		u32 epoch = max77620_rtc_date_to_epoch(&time) + (s32)n_cfg.timeoff;
		max77620_rtc_epoch_to_date(epoch, &time);
	}
	s_printf(fname, "%04d%02d%02d_%02d%02d%02d", time.year, time.month, time.day, time.hour, time.min, time.sec);
	s_printf(path + strlen(path), "/scr%s.bmp", fname);

	// Save screenshot and log.
	int res = sd_save_to_file(bitmap, file_size, path);
	if (!res)
		_save_log_to_bmp(fname);

	sd_unmount();

	free(bitmap);
	free(fb);

	if (!res)
		lv_mbox_set_text(mbox, mbox_scrsh_sav_ok);
	else
		lv_mbox_set_text(mbox, mbox_scrsh_fail);
	manual_system_maintenance(true);
	lv_mbox_start_auto_close(mbox, 4000);

	// Set timer to 2s.
	timer = get_tmr_ms() + 2000;
}

static void _disp_fb_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
	// Draw to framebuffer.
	gfx_set_rect_land_pitch((u32 *)NYX_FB_ADDRESS, (u32 *)color_p, 720, x1, y1, x2, y2); //pitch

	// Check if display init was done. If it's the first big draw, init.
	if (!disp_init_done && ((x2 - x1 + 1) > 600))
	{
		disp_init_done = true;
		_nyx_disp_init();
	}

	lv_flush_ready();
}

static touch_event touchpad;
static bool touch_enabled;
static bool console_enabled = false;

static bool _fts_touch_read(lv_indev_data_t *data)
{
	if (touch_enabled)
		touch_poll(&touchpad);
	else
		return false;

	// Take a screenshot if 3 fingers.
	if (touchpad.fingers > 2)
	{
		_save_fb_to_bmp();

		data->state = LV_INDEV_STATE_REL;
		return false;
	}

	if (console_enabled)
	{
		gfx_con_getpos(&gfx_con.savedx, &gfx_con.savedy);
		gfx_con_setpos(32, 638);
		gfx_con.fntsz = 8;
		gfx_printf("x: %4d, y: %4d | z: %3d | ", touchpad.x, touchpad.y, touchpad.z);
		gfx_printf("1: %02x, 2: %02x, 3: %02x, ", touchpad.raw[1], touchpad.raw[2], touchpad.raw[3]);
		gfx_printf("4: %02X, 5: %02x, 6: %02x, 7: %02x",
			touchpad.raw[4], touchpad.raw[5], touchpad.raw[6], touchpad.raw[7]);
		gfx_con_setpos(gfx_con.savedx, gfx_con.savedy);
		gfx_con.fntsz = 16;

		return false;
	}

	// Always set touch points.
	data->point.x = touchpad.x;
	data->point.y = touchpad.y;

	// Decide touch enable.
	switch (touchpad.type & STMFTS_MASK_EVENT_ID)
	{
	case STMFTS_EV_MULTI_TOUCH_ENTER:
	case STMFTS_EV_MULTI_TOUCH_MOTION:
		data->state = LV_INDEV_STATE_PR;
		break;
	case STMFTS_EV_MULTI_TOUCH_LEAVE:
		data->state = LV_INDEV_STATE_REL;
		break;
	case STMFTS_EV_NO_EVENT:
	default:
		if (touchpad.touch)
			data->state = LV_INDEV_STATE_PR;
		else
			data->state = LV_INDEV_STATE_REL;
		break;
	}

	return false; // No buffering so no more data read.
}

static lv_obj_t* list;
static lv_obj_t* btn_back;


static bool _jc_virt_mouse_read(lv_indev_data_t* data)
{
	// Poll Joy-Con.
	jc_gamepad_rpt_t* jc_pad = joycon_poll();

	if (!jc_pad)
	{
		data->state = LV_INDEV_STATE_REL;
		return false;
	}

	// Take a screenshot if Capture button is pressed.
	if (jc_pad->cap)
	{
		_save_fb_to_bmp();
		msleep(1000);
	}

	// Calibrate left stick.
	if (!jc_drv_ctx.centering_done)
	{
		if (jc_pad->conn_l
			&& jc_pad->lstick_x > 0x400 && jc_pad->lstick_y > 0x400
			&& jc_pad->lstick_x < 0xC00 && jc_pad->lstick_y < 0xC00)
		{
			jc_drv_ctx.cx_max = jc_pad->lstick_x + 0x72;
			jc_drv_ctx.cx_min = jc_pad->lstick_x - 0x72;
			jc_drv_ctx.cy_max = jc_pad->lstick_y + 0x72;
			jc_drv_ctx.cy_min = jc_pad->lstick_y - 0x72;
			jc_drv_ctx.centering_done = true;
			jc_drv_ctx.cursor_timeout = 0;
		}
		else
		{
			data->state = LV_INDEV_STATE_REL;
			return false;
		}
	}

	// Re-calibrate on disconnection.
	if (!jc_pad->conn_l)
		jc_drv_ctx.centering_done = 0;

	// Set button presses. //Buttons Joycon Funktion anwählen
	if (jc_pad->a || jc_pad->zl || jc_pad->zr)
		data->state = LV_INDEV_STATE_PR;
	else
		data->state = LV_INDEV_STATE_REL;

	// Enable console. //Buttons Joycon Debug Konsole aktivieren
	if (jc_pad->plus || jc_pad->minus)
	{
		if (((u32)get_tmr_ms() - jc_drv_ctx.console_timeout) > 1000)
		{
			if (!console_enabled)
			{
				display_activate_console();
				console_enabled = true;
				gfx_con_getpos(&gfx_con.savedx, &gfx_con.savedy);
				gfx_con_setpos(964, 630);
				gfx_printf("Press -/+ to close");
				gfx_con_setpos(gfx_con.savedx, gfx_con.savedy);
			}
			else
			{
				display_deactivate_console();
				console_enabled = false;
			}

			jc_drv_ctx.console_timeout = get_tmr_ms();
		}

		data->state = LV_INDEV_STATE_REL;
		return false;
	}

	if (console_enabled)
	{
		gfx_con_getpos(&gfx_con.savedx, &gfx_con.savedy);
		gfx_con_setpos(32, 630);
		gfx_con.fntsz = 8;
		gfx_printf("x: %4X, y: %4X | b: %06X | bt: %d %0d | cx: %03X - %03x, cy: %03X - %03x",
			jc_pad->lstick_x, jc_pad->lstick_y, jc_pad->buttons,
			jc_pad->batt_info_l, jc_pad->batt_info_r,
			jc_drv_ctx.cx_min, jc_drv_ctx.cx_max, jc_drv_ctx.cy_min, jc_drv_ctx.cy_max);
		gfx_con_setpos(gfx_con.savedx, gfx_con.savedy);
		gfx_con.fntsz = 16;

		data->state = LV_INDEV_STATE_REL;
		return false;
	}

	// Calculate new cursor position.
	if (jc_pad->lstick_x <= jc_drv_ctx.cx_max && jc_pad->lstick_x >= jc_drv_ctx.cx_min)
		jc_drv_ctx.pos_x += 0;
	else if (jc_pad->lstick_x > jc_drv_ctx.cx_max)
		jc_drv_ctx.pos_x += ((jc_pad->lstick_x - jc_drv_ctx.cx_max) / 30);
	else
		jc_drv_ctx.pos_x -= ((jc_drv_ctx.cx_min - jc_pad->lstick_x) / 30);

	if (jc_pad->lstick_y <= jc_drv_ctx.cy_max && jc_pad->lstick_y >= jc_drv_ctx.cy_min)
		jc_drv_ctx.pos_y += 0;
	else if (jc_pad->lstick_y > jc_drv_ctx.cy_max)
		jc_drv_ctx.pos_y -= ((jc_pad->lstick_y - jc_drv_ctx.cy_max) / 30);
	else
		jc_drv_ctx.pos_y += ((jc_drv_ctx.cy_min - jc_pad->lstick_y) / 30);

	// Ensure value inside screen limits.
	if (jc_drv_ctx.pos_x < 0)
		jc_drv_ctx.pos_x = 0;
	else if (jc_drv_ctx.pos_x > 1279)
		jc_drv_ctx.pos_x = 1279;

	if (jc_drv_ctx.pos_y < 0)
		jc_drv_ctx.pos_y = 0;
	else if (jc_drv_ctx.pos_y > 719)
		jc_drv_ctx.pos_y = 719;

	// Set cursor position.
	data->point.x = jc_drv_ctx.pos_x;
	data->point.y = jc_drv_ctx.pos_y;

	// Auto hide cursor.
	if (jc_drv_ctx.pos_x != jc_drv_ctx.pos_last_x || jc_drv_ctx.pos_y != jc_drv_ctx.pos_last_y)
	{
		jc_drv_ctx.pos_last_x = jc_drv_ctx.pos_x;
		jc_drv_ctx.pos_last_y = jc_drv_ctx.pos_y;

		jc_drv_ctx.cursor_hidden = false;
		jc_drv_ctx.cursor_timeout = get_tmr_ms();
		lv_indev_set_cursor(jc_drv_ctx.indev, jc_drv_ctx.cursor);

		// Un hide cursor.
		lv_obj_set_opa_scale_enable(jc_drv_ctx.cursor, false);
	}
	else
	{
		if (!jc_drv_ctx.cursor_hidden)
		{
			if (((u32)get_tmr_ms() - jc_drv_ctx.cursor_timeout) > 3000)
			{
				// Remove cursor and hide it.
				lv_indev_set_cursor(jc_drv_ctx.indev, NULL);
				lv_obj_set_opa_scale_enable(jc_drv_ctx.cursor, true);
				lv_obj_set_opa_scale(jc_drv_ctx.cursor, LV_OPA_TRANSP);

				jc_drv_ctx.cursor_hidden = true;
			}
		}
		else
			data->state = LV_INDEV_STATE_REL; // Ensure that no clicks are allowed.
	}

	if (jc_pad->x && close_btn)
	{
		lv_action_t close_btn_action = lv_btn_get_action(close_btn, LV_BTN_ACTION_CLICK);
		close_btn_action(close_btn);
		close_btn = NULL;

	}

	if (jc_pad->x && close_firstwin)
	{
		lv_action_t close_btn_action = lv_btn_get_action(close_firstwin, LV_BTN_ACTION_CLICK);
		close_btn_action(close_firstwin);
		close_firstwin = NULL;

	}

	if (jc_pad->b && btn_back)
	{
		lv_action_t back_btn_action = lv_btn_get_action(btn_back, LV_BTN_ACTION_CLICK);
		back_btn_action(btn_back);
		msleep(670);
	}

	if (jc_pad->a && list)
	{
		if (jc_drv_ctx.cursor_hidden)
		{
			if (lv_list_get_btn_selected(list) != NULL)
			{
				lv_action_t enter_action = lv_btn_get_action(lv_list_get_btn_selected(list), LV_BTN_ACTION_CLICK);
				enter_action(lv_list_get_btn_selected(list));
				msleep(670);
			}
		}
	}

	if (jc_pad->up && list)
	{

		lv_list_set_btn_selected(list, lv_list_get_prev_btn(list, lv_list_get_btn_selected(list)));
	}

	if (jc_pad->down && list)
	{

		lv_list_set_btn_selected(list, lv_list_get_next_btn(list, lv_list_get_btn_selected(list)));
	}

	if (jc_drv_ctx.cursor_hidden && list)
	{
		if (!jc_pad->y && list)
		{
			msleep(130);
			return false;
		}
	}

	u8 btn = btn_read();

	if (btn & BTN_POWER)
	{
		reload_nyx();
	}

	return false;
}


typedef struct _system_maintenance_tasks_t
{
	union
	{
		lv_task_t *tasks[2];
		struct
		{
			lv_task_t *status_bar;
			lv_task_t *dram_periodic_comp;
		} task;
	};
} system_maintenance_tasks_t;

static system_maintenance_tasks_t system_tasks;

void manual_system_maintenance(bool refresh)
{
	for (u32 task_idx = 0; task_idx < (sizeof(system_maintenance_tasks_t) / sizeof(lv_task_t *)); task_idx++)
	{
		lv_task_t *task = system_tasks.tasks[task_idx];
		if(task && (lv_tick_elaps(task->last_run) >= task->period))
		{
			task->last_run = lv_tick_get();
			task->task(task->param);
		}
	}
	if (refresh)
		lv_refr_now();
}

lv_img_dsc_t *bmp_to_lvimg_obj(const char *path)
{
	u32 fsize;
	u8 *bitmap = sd_file_read(path, &fsize);
	if (!bitmap)
		return NULL;

	struct _bmp_data
	{
		u32 size;
		u32 size_x;
		u32 size_y;
		u32 offset;
	};

	struct _bmp_data bmpData;

	// Get values manually to avoid unaligned access.
	bmpData.size = bitmap[2] | bitmap[3] << 8 |
		bitmap[4] << 16 | bitmap[5] << 24;
	bmpData.offset = bitmap[10] | bitmap[11] << 8 |
		bitmap[12] << 16 | bitmap[13] << 24;
	bmpData.size_x = bitmap[18] | bitmap[19] << 8 |
		bitmap[20] << 16 | bitmap[21] << 24;
	bmpData.size_y = bitmap[22] | bitmap[23] << 8 |
		bitmap[24] << 16 | bitmap[25] << 24;
	// Sanity check.
	if (bitmap[0] == 'B' &&
		bitmap[1] == 'M' &&
		bitmap[28] == 32 && // Only 32 bit BMPs allowed.
		bmpData.size <= fsize)
	{
		// Check if non-default Bottom-Top.
		bool flipped = false;
		if (bmpData.size_y & 0x80000000)
		{
			bmpData.size_y = ~(bmpData.size_y) + 1;
			flipped = true;
		}

		lv_img_dsc_t *img_desc = (lv_img_dsc_t *)bitmap;
		u32 offset_copy = ALIGN((u32)bitmap + sizeof(lv_img_dsc_t), 0x10);

		img_desc->header.always_zero = 0;
		img_desc->header.w = bmpData.size_x;
		img_desc->header.h = bmpData.size_y;
		img_desc->header.cf = (bitmap[28] == 32) ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
		img_desc->data_size = bmpData.size - bmpData.offset;
		img_desc->data = (u8 *)offset_copy;

		u32 *tmp = malloc(bmpData.size);
		u32 *tmp2 = (u32 *)offset_copy;

		// Copy the unaligned data to an aligned buffer.
		memcpy((u8 *)tmp, bitmap + bmpData.offset, img_desc->data_size);
		u32 j = 0;

		if (!flipped)
		{
			for (u32 y = 0; y < bmpData.size_y; y++)
			{
				for (u32 x = 0; x < bmpData.size_x; x++)
					tmp2[j++] = tmp[(bmpData.size_y - 1 - y ) * bmpData.size_x + x];
			}
		}
		else
		{
			for (u32 y = 0; y < bmpData.size_y; y++)
			{
				for (u32 x = 0; x < bmpData.size_x; x++)
					tmp2[j++] = tmp[y * bmpData.size_x + x];
			}
		}

		free(tmp);
	}
	else
	{
		free(bitmap);
		return NULL;
	}

	return (lv_img_dsc_t *)bitmap;
}

lv_res_t nyx_generic_onoff_toggle(lv_obj_t *btn)
{
	lv_obj_t *label_btn = lv_obj_get_child(btn, NULL);
	lv_obj_t *label_btn2 = lv_obj_get_child(btn, label_btn);

	char label_text[64];
	if (!label_btn2)
	{
		strcpy(label_text, lv_label_get_text(label_btn));
		label_text[strlen(label_text) - 15] = 0;

		if (!(lv_btn_get_state(btn) & LV_BTN_STATE_TGL_REL))
		{
			strcat(label_text, "#D0D0D0    OFF#");
			lv_label_set_text(label_btn, label_text);
		}
		else
		{
			s_printf(label_text, "%s%s%s", label_text, text_color, "    ON #");
			lv_label_set_text(label_btn, label_text);
		}
	}
	else
	{
		if (!(lv_btn_get_state(btn) & LV_BTN_STATE_TGL_REL))
			lv_label_set_text(label_btn, "#D0D0D0 OFF#");
		else
		{
			s_printf(label_text, "%s%s", text_color, " ON #");
			lv_label_set_text(label_btn, label_text);
		}
	}

	return LV_RES_OK;
}

lv_res_t mbox_action(lv_obj_t *btns, const char *txt)
{
	lv_obj_t *mbox = lv_mbox_get_from_btn(btns);
	lv_obj_t *dark_bg = lv_obj_get_parent(mbox);

	lv_obj_del(dark_bg); // Deletes children also (mbox).

	return LV_RES_INV;
}

bool nyx_emmc_check_battery_enough()
{
	if (fuse_read_hw_state() == FUSE_NX_HW_STATE_DEV)
		return true;

	int batt_volt = 0;

	max17050_get_property(MAX17050_VCELL, &batt_volt);

	if (batt_volt && batt_volt < 3650)
	{
		lv_obj_t *dark_bg = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_style(dark_bg, &mbox_darken);
		lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

		static const char * mbox_btn_map[] = { "\211", "\222OK", "\211", "" };
		lv_obj_t * mbox = lv_mbox_create(dark_bg, NULL);
		lv_mbox_set_recolor_text(mbox, true);

		lv_mbox_set_text(mbox,
			"#FF8000 Battery Check#\n\n"
			"#FFDD00 Battery is not enough to carry on#\n"
			"#FFDD00 with selected operation!#\n\n"
			"Charge to at least #C7EA46 3650 mV#, and try again!");

		lv_mbox_add_btns(mbox, mbox_btn_map, mbox_action);
		lv_obj_set_width(mbox, LV_HOR_RES / 9 * 5);
		lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_top(mbox, true);

		return false;
	}

	return true;
}

static void _nyx_sd_card_issues(void *param)
{
	lv_obj_t *dark_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_style(dark_bg, &mbox_darken);
	lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

	static const char * mbox_btn_map[] = { "\211", "\222OK", "\211", "" };
	lv_obj_t * mbox = lv_mbox_create(dark_bg, NULL);
	lv_mbox_set_recolor_text(mbox, true);

	lv_mbox_set_text(mbox,
		"#FF8000 SD Card Issues Check#\n\n"
		"#FFDD00 The SD Card is initialized in 1-bit mode!#\n"
		"#FFDD00 This might mean detached or broken connector!#\n\n"
		"You might want to check\n#C7EA46 Console Info# -> #C7EA46 microSD#");

	lv_mbox_add_btns(mbox, mbox_btn_map, mbox_action);
	lv_obj_set_width(mbox, LV_HOR_RES / 9 * 5);
	lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_top(mbox, true);
}

void nyx_window_toggle_buttons(lv_obj_t *win, bool disable)
{
	lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
	lv_obj_t * hbtn;

	hbtn = lv_obj_get_child_back(ext->header, NULL);
	hbtn = lv_obj_get_child_back(ext->header, hbtn); // Skip the title.

	if (disable)
	{
		while (hbtn != NULL)
		{
			lv_obj_set_opa_scale(hbtn, LV_OPA_40);
			lv_obj_set_opa_scale_enable(hbtn, true);
			lv_obj_set_click(hbtn, false);
			hbtn = lv_obj_get_child_back(ext->header, hbtn);
		}
	}
	else
	{
		while (hbtn != NULL)
		{
			lv_obj_set_opa_scale(hbtn, LV_OPA_COVER);
			lv_obj_set_click(hbtn, true);
			hbtn = lv_obj_get_child_back(ext->header, hbtn);
		}
	}
}

lv_res_t lv_win_close_action_custom(lv_obj_t* btn)
{
	close_btn = NULL;

	return lv_win_close_action(btn);
}

lv_obj_t* nyx_create_standard_window(const char* win_title)
{
	static lv_style_t win_bg_style;

	lv_style_copy(&win_bg_style, &lv_style_plain);
	win_bg_style.body.main_color = lv_theme_get_current()->bg->body.main_color;
	win_bg_style.body.grad_color = win_bg_style.body.main_color;

	lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, win_title);
	lv_win_set_style(win, LV_WIN_STYLE_BG, &win_bg_style);
	lv_obj_set_size(win, LV_HOR_RES, LV_VER_RES);

	close_btn = lv_win_add_btn(win, NULL, SYMBOL_CLOSE" Close", lv_win_close_action_custom);

	return win;
}

lv_obj_t *nyx_create_window_custom_close_btn(const char *win_title, lv_action_t rel_action)
{
	static lv_style_t win_bg_style;

	lv_style_copy(&win_bg_style, &lv_style_plain);
	win_bg_style.body.main_color = lv_theme_get_current()->bg->body.main_color;
	win_bg_style.body.grad_color = win_bg_style.body.main_color;

	lv_obj_t *win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, win_title);
	lv_win_set_style(win, LV_WIN_STYLE_BG, &win_bg_style);
	lv_obj_set_size(win, LV_HOR_RES, LV_VER_RES);

	close_btn = lv_win_add_btn(win, NULL, SYMBOL_CLOSE" Close", rel_action);

	return win;
}

void reload_nyx()
{
	b_cfg->boot_cfg = BOOT_CFG_AUTOBOOT_EN;
	b_cfg->autoboot = 0;
	b_cfg->autoboot_list = 0;
	b_cfg->extra_cfg = 0;

	void (*main_ptr)() = (void *)nyx_str->hekate;

	sd_end();

	hw_reinit_workaround(false, 0);

	// Some cards (Sandisk U1), do not like a fast power cycle. Wait min 100ms.
	sdmmc_storage_init_wait_sd();

	(*main_ptr)();
}

static lv_res_t reload_action(lv_obj_t *btns, const char *txt)
{
	if (!lv_btnm_get_pressed(btns))
		reload_nyx();

	return mbox_action(btns, txt);
}

static lv_res_t _removed_sd_action(lv_obj_t *btns, const char *txt)
{
	u32 btnidx = lv_btnm_get_pressed(btns);

	switch (btnidx)
	{
	case 0:
		if (h_cfg.rcm_patched)
			power_set_state(POWER_OFF_REBOOT);
		else
			power_set_state(REBOOT_RCM);
		break;
	case 1:
		power_set_state(POWER_OFF_RESET);
		break;
	case 2:
		sd_end();
		do_reload = false;
		break;
	}

	return mbox_action(btns, txt);
}

static void _check_sd_card_removed(void *params)
{
	// The following checks if SDMMC_1 is initialized.
	// If yes and card was removed, shows a message box,
	// that will reload Nyx, when the card is inserted again.
	if (!do_reload && sd_get_card_removed())
	{
		lv_obj_t *dark_bg = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_style(dark_bg, &mbox_darken);
		lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

		static const char * mbox_btn_map[] = { "\221Reboot (RCM)", "\221Power Off", "\221Do not reload", "" };
		static const char * mbox_btn_map_rcm_patched[] = { "\221Reboot", "\221Power Off", "\221Do not reload", "" };
		lv_obj_t *mbox = lv_mbox_create(dark_bg, NULL);
		lv_mbox_set_recolor_text(mbox, true);
		lv_obj_set_width(mbox, LV_HOR_RES * 6 / 9);

		lv_mbox_set_text(mbox, "\n#FF8000 SD card was removed!#\n\n#96FF00 Nyx will reload after inserting it.#\n");
		lv_mbox_add_btns(mbox, h_cfg.rcm_patched ? mbox_btn_map_rcm_patched : mbox_btn_map, _removed_sd_action);

		lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_top(mbox, true);

		do_reload = true;
	}

	// If in reload state and card was inserted, reload nyx.
	if (do_reload && !sd_get_card_removed())
		reload_nyx();
}

lv_task_t *task_emmc_errors;
static void _nyx_emmc_issues(void *params)
{
	if (emmc_get_mode() < EMMC_MMC_HS400)
	{
		// Remove task.
		lv_task_del(task_emmc_errors);

		lv_obj_t *dark_bg = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_style(dark_bg, &mbox_darken);
		lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

		static const char * mbox_btn_map[] = { "\211", "\222OK", "\211", "" };
		lv_obj_t * mbox = lv_mbox_create(dark_bg, NULL);
		lv_mbox_set_recolor_text(mbox, true);

		lv_mbox_set_text(mbox,
			"#FF8000 eMMC Issues Check#\n\n"
			"#FFDD00 Your eMMC is initialized in slower mode!#\n"
			"#FFDD00 This might mean hardware issues!#\n\n"
			"You might want to check\n#C7EA46 Console Info# -> #C7EA46 eMMC#");

		lv_mbox_add_btns(mbox, mbox_btn_map, mbox_action);
		lv_obj_set_width(mbox, LV_HOR_RES / 9 * 5);
		lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_top(mbox, true);
	}
}


static lv_res_t _create_mbox_reload(lv_obj_t* btn)
{
	static lv_style_t mbox_bg;
	lv_style_copy(&mbox_bg, &lv_style_pretty);
	mbox_bg.body.main_color = LV_COLOR_BLACK;
	mbox_bg.body.grad_color = mbox_darken.body.main_color;
	mbox_bg.body.opa = LV_OPA_40;

	lv_obj_t* dark_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_style(dark_bg, &mbox_darken);
	lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

	static const char* mbox_btn_map[] = { "\221Reload", "\221Cancel", "" };
	lv_obj_t* mbox = lv_mbox_create(dark_bg, NULL);
	lv_mbox_set_recolor_text(mbox, true);
	lv_obj_set_width(mbox, LV_HOR_RES * 4 / 10);
	lv_mbox_set_text(mbox, mbox_reload);
	lv_mbox_add_btns(mbox, mbox_btn_map, reload_action);

	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_BG, &lv_style_transp);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_REL, &btn_transp_rel);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_PR, &btn_transp_pr);

	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &mbox_bg);

	lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_top(mbox, true);

	return LV_RES_OK;
}

void nyx_create_onoff_button(lv_theme_t *th, lv_obj_t *parent, lv_obj_t *btn, const char *btn_name, lv_action_t action, bool transparent)
{
	// Create buttons that are flat and text, plus On/Off switch.
	static lv_style_t btn_onoff_rel_hos_style, btn_onoff_pr_hos_style;
	lv_style_copy(&btn_onoff_rel_hos_style, th->btn.rel);
	btn_onoff_rel_hos_style.body.shadow.width = 0;
	btn_onoff_rel_hos_style.body.border.width = 0;
	btn_onoff_rel_hos_style.body.padding.hor = 0;
	btn_onoff_rel_hos_style.body.radius = 0;
	btn_onoff_rel_hos_style.body.empty = 1;

	lv_style_copy(&btn_onoff_pr_hos_style, &btn_onoff_rel_hos_style);
	if (transparent)
	{
		btn_onoff_pr_hos_style.body.main_color = LV_COLOR_HEX(0xFFFFFF);
		btn_onoff_pr_hos_style.body.opa = 35;
	}
	else
		btn_onoff_pr_hos_style.body.main_color = LV_COLOR_HEX(0x3D3D3D);
	btn_onoff_pr_hos_style.body.grad_color = btn_onoff_pr_hos_style.body.main_color;
	btn_onoff_pr_hos_style.text.color = th->btn.pr->text.color;
	btn_onoff_pr_hos_style.body.empty = 0;

	lv_obj_t *label_btn = lv_label_create(btn, NULL);
	lv_obj_t *label_btnsw = NULL;

	lv_label_set_recolor(label_btn, true);
	label_btnsw = lv_label_create(btn, NULL);
	lv_label_set_recolor(label_btnsw, true);
	lv_btn_set_layout(btn, LV_LAYOUT_OFF);

	lv_btn_set_style(btn, LV_BTN_STYLE_REL, &btn_onoff_rel_hos_style);
	lv_btn_set_style(btn, LV_BTN_STYLE_PR, &btn_onoff_pr_hos_style);
	lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, &btn_onoff_rel_hos_style);
	lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, &btn_onoff_pr_hos_style);

	lv_btn_set_fit(btn, false, true);
	lv_obj_set_width(btn, lv_obj_get_width(parent));
	lv_btn_set_toggle(btn, true);

	lv_label_set_text(label_btn, btn_name);

	lv_label_set_text(label_btnsw, "#D0D0D0 OFF#");
	lv_obj_align(label_btn, btn, LV_ALIGN_IN_LEFT_MID, LV_DPI / 4, 0);
	lv_obj_align(label_btnsw, btn, LV_ALIGN_IN_RIGHT_MID, -LV_DPI / 4, -LV_DPI / 10);

	if (action)
		lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, action);
}

#include <libs/lvgl/lv_themes/lv_theme_storm.h>

const char* infotext = "";
const char* sprache = "";

lv_color_t c_weiss;
lv_color_t c_schwarz;
lv_color_t c_silber;
lv_color_t c_grau;

static lv_color_t color_label;
static lv_color_t color_tab_label;
static lv_color_t color_pl_icon;

void create_color_btn(lv_obj_t* parent, lv_obj_t* btn, lv_color_t color, lv_action_t action)
{
	lv_style_t* btn_onoff_rel_hos_style = malloc(sizeof(lv_style_t));
	lv_style_t* btn_onoff_pr_hos_style = malloc(sizeof(lv_style_t));
	lv_style_copy(btn_onoff_rel_hos_style, lv_theme_get_current()->btn.rel);
	btn_onoff_rel_hos_style->body.main_color = color;
	btn_onoff_rel_hos_style->body.grad_color = btn_onoff_rel_hos_style->body.main_color;
	btn_onoff_rel_hos_style->body.padding.hor = 0;
	btn_onoff_rel_hos_style->body.radius = 0;

	lv_style_copy(btn_onoff_pr_hos_style, lv_theme_get_current()->btn.pr);
	btn_onoff_pr_hos_style->body.main_color = color;
	btn_onoff_pr_hos_style->body.grad_color = btn_onoff_pr_hos_style->body.main_color;
	btn_onoff_pr_hos_style->body.padding.hor = 0;
	btn_onoff_pr_hos_style->body.border.color = LV_COLOR_GRAY;
	btn_onoff_pr_hos_style->body.border.width = 4;
	btn_onoff_pr_hos_style->body.radius = 0;

	lv_btn_set_style(btn, LV_BTN_STYLE_REL, btn_onoff_rel_hos_style);
	lv_btn_set_style(btn, LV_BTN_STYLE_PR, btn_onoff_pr_hos_style);
	lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, btn_onoff_rel_hos_style);
	lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, btn_onoff_pr_hos_style);

	lv_btn_set_fit(btn, false, true);
	lv_obj_set_width(btn, lv_obj_get_height(btn));
	lv_btn_set_toggle(btn, true);

	if (action)
		lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, action);
}

typedef struct _color_akt_ctxt
{
	u16 hue;
	u16 hue_tab;
	u16 hue_pl_icon;
	lv_obj_t* label;
	lv_obj_t* tab_label;
	lv_obj_t* pl_icons;
	lv_obj_t* pl_icons_info;
	u16 red;
	u16 green;
	u16 blue;
	u16 red_tab;
	u16 green_tab;
	u16 blue_tab;
	u16 red_pl_icon;
	u16 green_pl_icon;
	u16 blue_pl_icon;
	int hue_or_hex_label;
	int hue_or_hex_tab_label;
	int hue_or_hex_pl_icon;
} color_akt_ctxt;

color_akt_ctxt color_akt;

static void test_color_label(u16 hue)
{
	lv_color_t color = lv_color_hsv_to_rgb(hue, 100, 100);
	static lv_style_t txt_test;
	lv_style_copy(&txt_test, lv_label_get_style(color_akt.label));
	txt_test.text.color = color;
	lv_obj_set_style(color_akt.label, &txt_test);
}

static void test_color_tab(u16 hue)
{
	lv_color_t color = lv_color_hsv_to_rgb(hue, 100, 100);
	static lv_style_t txt_tab_test;
	lv_style_copy(&txt_tab_test, lv_label_get_style(color_akt.tab_label));
	txt_tab_test.text.color = color;
	lv_obj_set_style(color_akt.tab_label, &txt_tab_test);
}

static void test_color_pl_icon(u16 hue)
{
	lv_color_t color = lv_color_hsv_to_rgb(hue, 100, 100);
	static lv_style_t pl_icon_test;
	lv_style_copy(&pl_icon_test, lv_label_get_style(color_akt.pl_icons));
	pl_icon_test.text.color = color;
	lv_obj_set_style(color_akt.pl_icons, &pl_icon_test);
	static lv_style_t pl_icon_test_info;
	lv_style_copy(&pl_icon_test_info, lv_label_get_style(color_akt.pl_icons_info));
	pl_icon_test_info.text.color = color;
	lv_obj_set_style(color_akt.pl_icons_info, &pl_icon_test_info);
}

static lv_res_t preset_hue_action_label(lv_obj_t* btn)
{
	lv_btn_ext_t* ext = lv_obj_get_ext_attr(btn);
	color_akt.hue = ext->idx;
	test_color_label(color_akt.hue);
	color_akt.hue_or_hex_label = 1;
	return LV_RES_OK;
}

static lv_res_t preset_hue_action_tab_label(lv_obj_t* btn)
{
	lv_btn_ext_t* ext = lv_obj_get_ext_attr(btn);
	color_akt.hue_tab = ext->idx;
	test_color_tab(color_akt.hue_tab);
	color_akt.hue_or_hex_tab_label = 1;
	return LV_RES_OK;
}

static lv_res_t preset_hue_action_pl_icon(lv_obj_t* btn)
{
	lv_btn_ext_t* ext = lv_obj_get_ext_attr(btn);
	color_akt.hue_pl_icon = ext->idx;
	test_color_pl_icon(color_akt.hue_pl_icon);
	color_akt.hue_or_hex_pl_icon = 1;
	return LV_RES_OK;
}

static void test_hex_color_label()
{
	lv_color_t color = LV_COLOR_MAKE(color_akt.red, color_akt.green, color_akt.blue);
	static lv_style_t txt_test;
	lv_style_copy(&txt_test, lv_label_get_style(color_akt.label));
	txt_test.text.color = color;
	lv_obj_set_style(color_akt.label, &txt_test);
}

static void test_hex_color_tab()
{
	lv_color_t color = LV_COLOR_MAKE(color_akt.red_tab, color_akt.green_tab, color_akt.blue_tab);
	static lv_style_t txt_tab_test;
	lv_style_copy(&txt_tab_test, lv_label_get_style(color_akt.tab_label));
	txt_tab_test.text.color = color;
	lv_obj_set_style(color_akt.tab_label, &txt_tab_test);
}

static void test_hex_color_pl_icon()
{
	lv_color_t color = LV_COLOR_MAKE(color_akt.red_pl_icon, color_akt.green_pl_icon, color_akt.blue_pl_icon);
	static lv_style_t pl_icon_test;
	lv_style_copy(&pl_icon_test, lv_label_get_style(color_akt.pl_icons));
	pl_icon_test.text.color = color;
	lv_obj_set_style(color_akt.pl_icons, &pl_icon_test);
	static lv_style_t pl_icon_test_info;
	lv_style_copy(&pl_icon_test_info, lv_label_get_style(color_akt.pl_icons_info));
	pl_icon_test_info.text.color = color;
	lv_obj_set_style(color_akt.pl_icons_info, &pl_icon_test_info);
}

static lv_res_t preset_hex_action_label(lv_obj_t* btn)
{
	lv_btn_ext_t* exthex = lv_obj_get_ext_attr(btn);

	if (exthex->idx == 1)
	{
		color_akt.red = c_weiss.red;
		color_akt.green = c_weiss.green;
		color_akt.blue = c_weiss.blue;
		test_hex_color_label();
	}

	if (exthex->idx == 2)
	{
		color_akt.red = c_silber.red;
		color_akt.green = c_silber.green;
		color_akt.blue = c_silber.blue;
		test_hex_color_label();
	}

	if (exthex->idx == 3)
	{
		color_akt.red = c_grau.red;
		color_akt.green = c_grau.green;
		color_akt.blue = c_grau.blue;
		test_hex_color_label();
	}

	if (exthex->idx == 4)
	{
		color_akt.red = c_schwarz.red;
		color_akt.green = c_schwarz.green;
		color_akt.blue = c_schwarz.blue;
		test_hex_color_label();
	}

	color_akt.hue_or_hex_label = 2;
	return LV_RES_OK;
}

static lv_res_t preset_hex_action_tab_label(lv_obj_t* btn)
{
	lv_btn_ext_t* exthex = lv_obj_get_ext_attr(btn);

	if (exthex->idx == 1)
	{
		color_akt.red_tab = c_weiss.red;
		color_akt.green_tab = c_weiss.green;
		color_akt.blue_tab = c_weiss.blue;
		test_hex_color_tab();
	}

	if (exthex->idx == 2)
	{
		color_akt.red_tab = c_silber.red;
		color_akt.green_tab = c_silber.green;
		color_akt.blue_tab = c_silber.blue;
		test_hex_color_tab();
	}

	if (exthex->idx == 3)
	{
		color_akt.red_tab = c_grau.red;
		color_akt.green_tab = c_grau.green;
		color_akt.blue_tab = c_grau.blue;
		test_hex_color_tab();
	}

	if (exthex->idx == 4)
	{
		color_akt.red_tab = c_schwarz.red;
		color_akt.green_tab = c_schwarz.green;
		color_akt.blue_tab = c_schwarz.blue;
		test_hex_color_tab();
	}

	color_akt.hue_or_hex_tab_label = 2;
	return LV_RES_OK;
}

static lv_res_t preset_hex_action_pl_icon(lv_obj_t* btn)
{
	lv_btn_ext_t* exthex = lv_obj_get_ext_attr(btn);

	if (exthex->idx == 1)
	{
		color_akt.red_pl_icon = c_weiss.red;
		color_akt.green_pl_icon = c_weiss.green;
		color_akt.blue_pl_icon = c_weiss.blue;
		test_hex_color_pl_icon();
	}

	if (exthex->idx == 2)
	{
		color_akt.red_pl_icon = c_silber.red;
		color_akt.green_pl_icon = c_silber.green;
		color_akt.blue_pl_icon = c_silber.blue;
		test_hex_color_pl_icon();
	}

	if (exthex->idx == 3)
	{
		color_akt.red_pl_icon = c_grau.red;
		color_akt.green_pl_icon = c_grau.green;
		color_akt.blue_pl_icon = c_grau.blue;
		test_hex_color_pl_icon();
	}

	if (exthex->idx == 4)
	{
		color_akt.red_pl_icon = c_schwarz.red;
		color_akt.green_pl_icon = c_schwarz.green;
		color_akt.blue_pl_icon = c_schwarz.blue;
		test_hex_color_pl_icon();
	}

	color_akt.hue_or_hex_pl_icon = 2;
	return LV_RES_OK;
}

static lv_res_t ctrl_colorsave(lv_obj_t* btn)
{
	sd_mount();
	FIL fp;

	f_unlink("argon/sys/color.ini");

	if (f_stat("argon/sys/color.ini", NULL)) {

		f_open(&fp, "argon/sys/color.ini", FA_CREATE_NEW);
		f_close(&fp);

	}

	f_open(&fp, "argon/sys/color.ini", FA_WRITE);

	if (color_akt.hue_or_hex_label == 1)
	{
		char hue[20];
		s_printf(hue, "Hue_lbl = %03d", color_akt.hue);
		f_puts(hue, &fp);
		f_puts("\n", &fp);

	}

	else
	{
		char hex_label[30];
		s_printf(hex_label, "Hex_lbl = %03d,%03d,%03d", color_akt.red, color_akt.green, color_akt.blue);
		f_puts(hex_label, &fp);
		f_puts("\n", &fp);
	}

	if (color_akt.hue_or_hex_tab_label == 1)
	{
		char hue_tab_label[20];
		s_printf(hue_tab_label, "Hue_tablbl = %03d", color_akt.hue_tab);
		f_puts(hue_tab_label, &fp);
		f_puts("\n", &fp);
	}

	else
	{
		char hex_tab_label[30];
		s_printf(hex_tab_label, "Hex_tablbl = %03d,%03d,%03d", color_akt.red_tab, color_akt.green_tab, color_akt.blue_tab);
		f_puts(hex_tab_label, &fp);
		f_puts("\n", &fp);
	}

	if (color_akt.hue_or_hex_pl_icon == 1)
	{
		char hue_pl_icon[20];
		s_printf(hue_pl_icon, "Hue_pl_icon = %03d", color_akt.hue_pl_icon);
		f_puts(hue_pl_icon, &fp);
	}

	else
	{
		char hex_pl_icon[30];
		s_printf(hex_pl_icon, "Hex_pl_icon = %03d,%03d,%03d", color_akt.red_pl_icon, color_akt.green_pl_icon, color_akt.blue_pl_icon);
		f_puts(hex_pl_icon, &fp);
	}

	f_close(&fp);

	sd_unmount(false);

	static lv_style_t bg;
	lv_style_copy(&bg, &lv_style_pretty);
	bg.text.color = color_label;
	bg.body.opa = LV_OPA_0;
	bg.text.font = &interui_20;

	lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
	lv_mbox_set_recolor(mbox, true);
	lv_obj_set_width(mbox, LV_DPI * 5);
	lv_obj_set_top(mbox, true);
	lv_obj_set_auto_realign(mbox, true);
	lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
	lv_mbox_set_text(mbox, mbox_colorchange_color_save);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
	lv_mbox_start_auto_close(mbox, 8000);

	return LV_RES_OK;
}

static lv_res_t ctrl_color_load(lv_obj_t* btn)
{
#define MAXCHAR 100
	FIL fp;
	int linien = 0;
	char color_read[MAXCHAR];
	sd_mount();

	if (f_stat("argon/sys/color.ini", NULL))
	{
		color_akt.red = c_weiss.red;
		color_akt.green = c_weiss.green;
		color_akt.blue = c_weiss.blue;
		color_akt.hue_or_hex_label = 2;
		color_akt.hue_tab = 231;
		color_akt.hue_or_hex_tab_label = 1;
		color_akt.red_pl_icon = c_weiss.red;
		color_akt.green_pl_icon = c_weiss.green;
		color_akt.blue_pl_icon = c_weiss.blue;
		color_akt.hue_or_hex_pl_icon = 2;
		static lv_style_t bg;
		lv_style_copy(&bg, &lv_style_pretty);
		bg.text.color = LV_COLOR_RED;
		bg.body.opa = LV_OPA_0;
		bg.text.font = &interui_20;
		lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
		lv_mbox_set_recolor(mbox, true);
		lv_obj_set_width(mbox, LV_DPI * 5);
		lv_obj_set_top(mbox, true);
		lv_obj_set_auto_realign(mbox, true);
		lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
		lv_mbox_set_text(mbox, mbox_colorchange_colorini_notfound);
		lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
		lv_mbox_start_auto_close(mbox, 8000);
	}

	else {

		f_open(&fp, "argon/sys/color.ini", FA_READ);
		while (f_gets(color_read, MAXCHAR, &fp)) {
			linien++;

			char* color_read_label = (char*)malloc(256);
			char* color_read_tab_label = (char*)malloc(256);
			char* color_read_pl_icon = (char*)malloc(256);

			if (strstr(color_read, "Hue_lbl =") != 0)
			{
				color_read_label = strstr(color_read, "Hue_lbl =");
				color_read_label[strlen(color_read_label) - 1] = '\0';
				char* stelle;
				char* delchar;
				stelle = strchr(color_read_label, (int)'=');
				delchar = str_replace(stelle, "= ", "");
				u16 hue_read = atoi(delchar);
				color_akt.hue = hue_read;
				color_akt.hue_or_hex_label = 1;
			}

			if (strstr(color_read, "Hex_lbl =") != 0)
			{
				color_read_label = strstr(color_read, "Hex_lbl =");
				color_read_label[strlen(color_read_label) - 1] = '\0';
				char* stelle;
				char* delchar;
				stelle = strchr(color_read_label, (int)'=');
				delchar = str_replace(stelle, "= ", "");
				char* hex_red = (char*)malloc(256);
				char* hex_green = (char*)malloc(256);
				char* hex_green_temp = (char*)malloc(256);
				char* hex_blue = (char*)malloc(256);
				strcpy(hex_red, delchar);
				hex_red[strlen(hex_red) - 8] = '\0';
				u16 hex_red_read = atoi(hex_red);
				strcpy(hex_green_temp, delchar + 4);
				strcpy(hex_green, hex_green_temp);
				hex_green[strlen(hex_green) - 4] = '\0';
				u16 hex_green_read = atoi(hex_green);
				strcpy(hex_blue, delchar + 8);
				u16 hex_blue_read = atoi(hex_blue);
				color_akt.red = hex_red_read;
				color_akt.green = hex_green_read;
				color_akt.blue = hex_blue_read;
				color_akt.hue_or_hex_label = 2;
			}

			if (strstr(color_read, "Hue_tablbl =") != 0)
			{
				color_read_tab_label = strstr(color_read, "Hue_tablbl =");
				color_read_tab_label[strlen(color_read_tab_label) - 1] = '\0';
				char* stelle;
				char* delchar;
				stelle = strchr(color_read_tab_label, (int)'=');
				delchar = str_replace(stelle, "= ", "");
				u16 hue_read = atoi(delchar);
				color_akt.hue_tab = hue_read;
				color_akt.hue_or_hex_tab_label = 1;
			}

			if (strstr(color_read, "Hex_tablbl =") != 0)
			{
				color_read_tab_label = strstr(color_read, "Hex_tablbl =");
				color_read_tab_label[strlen(color_read_tab_label) - 1] = '\0';
				char* stelle;
				char* delchar;
				stelle = strchr(color_read_tab_label, (int)'=');
				delchar = str_replace(stelle, "= ", "");
				char* hex_red = (char*)malloc(256);
				char* hex_green = (char*)malloc(256);
				char* hex_green_temp = (char*)malloc(256);
				char* hex_blue = (char*)malloc(256);
				strcpy(hex_red, delchar);
				hex_red[strlen(hex_red) - 8] = '\0';
				u16 hex_red_read = atoi(hex_red);
				strcpy(hex_green_temp, delchar + 4);
				strcpy(hex_green, hex_green_temp);
				hex_green[strlen(hex_green) - 4] = '\0';
				u16 hex_green_read = atoi(hex_green);
				strcpy(hex_blue, delchar + 8);
				u16 hex_blue_read = atoi(hex_blue);
				color_akt.red_tab = hex_red_read;
				color_akt.green_tab = hex_green_read;
				color_akt.blue_tab = hex_blue_read;
				color_akt.hue_or_hex_tab_label = 2;
			}

			if (strstr(color_read, "Hue_pl_icon =") != 0)
			{
				color_read_pl_icon = strstr(color_read, "Hue_pl_icon =");
				color_read_pl_icon[strlen(color_read_pl_icon) - 0] = '\0';
				char* stelle;
				char* delchar;
				stelle = strchr(color_read_pl_icon, (int)'=');
				delchar = str_replace(stelle, "= ", "");
				u16 hue_read = atoi(delchar);
				color_akt.hue_pl_icon = hue_read;
				color_akt.hue_or_hex_pl_icon = 1;
			}

			if (strstr(color_read, "Hex_pl_icon =") != 0)
			{
				color_read_pl_icon = strstr(color_read, "Hex_pl_icon =");
				color_read_pl_icon[strlen(color_read_pl_icon) - 0] = '\0';
				char* stelle;
				char* delchar;
				stelle = strchr(color_read_pl_icon, (int)'=');
				delchar = str_replace(stelle, "= ", "");
				char* hex_red = (char*)malloc(256);
				char* hex_green = (char*)malloc(256);
				char* hex_green_temp = (char*)malloc(256);
				char* hex_blue = (char*)malloc(256);
				strcpy(hex_red, delchar);
				hex_red[strlen(hex_red) - 8] = '\0';
				u16 hex_red_read = atoi(hex_red);
				strcpy(hex_green_temp, delchar + 4);
				strcpy(hex_green, hex_green_temp);
				hex_green[strlen(hex_green) - 4] = '\0';
				u16 hex_green_read = atoi(hex_green);
				strcpy(hex_blue, delchar + 8);
				u16 hex_blue_read = atoi(hex_blue);
				color_akt.red_pl_icon = hex_red_read;
				color_akt.green_pl_icon = hex_green_read;
				color_akt.blue_pl_icon = hex_blue_read;
				color_akt.hue_or_hex_pl_icon = 2;
			}
		}
	}

	f_close(&fp);
	sd_unmount(false);
	return LV_RES_OK;
}

const u16 colors[14] = {
	 291, 341, 4, 33, 43, 54, 66, 89, 124, 167, 187, 200, 208, 231
};

static lv_res_t ctrl_color_change(lv_obj_t* btn)
{
	lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, lbl_win_colorchange);
	lv_page_set_scrl_layout(lv_win_get_content(win), LV_LAYOUT_OFF);
	lv_win_set_style(win, LV_WIN_STYLE_HEADER, &header_style);
	lv_win_set_style(win, LV_WIN_STYLE_BG, &win_bg_style);

	close_btn = lv_win_add_btn(win, NULL, SYMBOL_CLOSE, lv_win_close_action_custom);
	lv_obj_set_style(close_btn, LV_LABEL_STYLE_MAIN);

	lv_obj_t* reload_btn = lv_win_add_btn(win, NULL, SYMBOL_REFRESH, _create_mbox_reload);
	lv_obj_set_style(reload_btn, LV_LABEL_STYLE_MAIN);

	lv_obj_t* save_btn = lv_win_add_btn(win, NULL, SYMBOL_SAVE, ctrl_colorsave);
	lv_obj_set_style(save_btn, LV_LABEL_STYLE_MAIN);

	lv_win_set_btn_size(win, 45);

	lv_obj_t* label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_label_colorchange);
	lv_obj_set_pos(label, 40, 20);
	lv_label_set_style(label, &labels_style);

	lv_obj_t* color_btn = lv_btn_create(win, NULL);
	lv_btn_ext_t* ext = lv_obj_get_ext_attr(color_btn);
	ext->idx = colors[0];

	create_color_btn(win, color_btn, lv_color_hsv_to_rgb(colors[0], 100, 100), preset_hue_action_label);
	lv_obj_align(color_btn, label, LV_ALIGN_OUT_LEFT_BOTTOM, 55, 80);

	lv_obj_t* color_btn2;

	for (u32 i = 1; i < 14; i++)
	{
		color_btn2 = lv_btn_create(win, NULL);
		ext = lv_obj_get_ext_attr(color_btn2);
		ext->idx = colors[i];
		create_color_btn(win, color_btn2, lv_color_hsv_to_rgb(colors[i], 100, 100), preset_hue_action_label);
		lv_obj_align(color_btn2, color_btn, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
		color_btn = color_btn2;
	}

	lv_obj_t* color_btn3 = lv_btn_create(win, NULL);
	lv_btn_ext_t* exthex = lv_obj_get_ext_attr(color_btn3);
	exthex->idx = 1;
	create_color_btn(win, color_btn3, c_weiss, preset_hex_action_label);
	lv_obj_align(color_btn3, color_btn2, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn4 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn4);
	exthex->idx = 2;
	create_color_btn(win, color_btn4, c_silber, preset_hex_action_label);
	lv_obj_align(color_btn4, color_btn3, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn5 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn5);
	exthex->idx = 3;
	create_color_btn(win, color_btn5, c_grau, preset_hex_action_label);
	lv_obj_align(color_btn5, color_btn4, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn6 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn6);
	exthex->idx = 4;
	create_color_btn(win, color_btn6, c_schwarz, preset_hex_action_label);
	lv_obj_align(color_btn6, color_btn5, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_tab_label_colorchange);
	lv_obj_set_pos(label, 40, 210);
	lv_label_set_style(label, &labels_style);

	lv_obj_t* color_btn0 = lv_btn_create(win, NULL);
	ext = lv_obj_get_ext_attr(color_btn0);
	ext->idx = colors[0];

	create_color_btn(win, color_btn0, lv_color_hsv_to_rgb(colors[0], 100, 100), preset_hue_action_tab_label);

	lv_obj_align(color_btn0, label, LV_ALIGN_OUT_LEFT_BOTTOM, 55, 80);

	lv_obj_t* color_btn02;

	for (u32 i = 1; i < 14; i++)
	{
		color_btn02 = lv_btn_create(win, NULL);
		ext = lv_obj_get_ext_attr(color_btn02);
		ext->idx = colors[i];
		create_color_btn(win, color_btn02, lv_color_hsv_to_rgb(colors[i], 100, 100), preset_hue_action_tab_label);
		lv_obj_align(color_btn02, color_btn0, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
		color_btn0 = color_btn02;
	}

	lv_obj_t* color_btn03 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn03);
	exthex->idx = 1;
	create_color_btn(win, color_btn03, c_weiss, preset_hex_action_tab_label);
	lv_obj_align(color_btn03, color_btn02, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn04 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn04);
	exthex->idx = 2;
	create_color_btn(win, color_btn04, c_silber, preset_hex_action_tab_label);
	lv_obj_align(color_btn04, color_btn03, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn05 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn05);
	exthex->idx = 3;
	create_color_btn(win, color_btn05, c_grau, preset_hex_action_tab_label);
	lv_obj_align(color_btn05, color_btn04, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn06 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn06);
	exthex->idx = 4;
	create_color_btn(win, color_btn06, c_schwarz, preset_hex_action_tab_label);
	lv_obj_align(color_btn06, color_btn05, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_pl_icon_colorchange);
	lv_obj_set_pos(label, 40, 405);
	lv_label_set_style(label, &labels_style);

	lv_obj_t* color_btn00 = lv_btn_create(win, NULL);
	ext = lv_obj_get_ext_attr(color_btn00);
	ext->idx = colors[0];

	create_color_btn(win, color_btn00, lv_color_hsv_to_rgb(colors[0], 100, 100), preset_hue_action_pl_icon);

	lv_obj_align(color_btn00, label, LV_ALIGN_OUT_LEFT_BOTTOM, 55, 80);

	lv_obj_t* color_btn002;

	for (u32 i = 1; i < 14; i++)
	{
		color_btn002 = lv_btn_create(win, NULL);
		ext = lv_obj_get_ext_attr(color_btn002);
		ext->idx = colors[i];
		create_color_btn(win, color_btn002, lv_color_hsv_to_rgb(colors[i], 100, 100), preset_hue_action_pl_icon);
		lv_obj_align(color_btn002, color_btn00, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
		color_btn00 = color_btn002;
	}

	lv_obj_t* color_btn003 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn003);
	exthex->idx = 1;
	create_color_btn(win, color_btn003, c_weiss, preset_hex_action_pl_icon);
	lv_obj_align(color_btn003, color_btn002, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn004 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn004);
	exthex->idx = 2;
	create_color_btn(win, color_btn004, c_silber, preset_hex_action_pl_icon);
	lv_obj_align(color_btn004, color_btn003, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn005 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn005);
	exthex->idx = 3;
	create_color_btn(win, color_btn005, c_grau, preset_hex_action_pl_icon);
	lv_obj_align(color_btn005, color_btn004, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* color_btn006 = lv_btn_create(win, NULL);
	exthex = lv_obj_get_ext_attr(color_btn006);
	exthex->idx = 4;
	create_color_btn(win, color_btn006, c_schwarz, preset_hex_action_pl_icon);
	lv_obj_align(color_btn006, color_btn005, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	lv_obj_t* lbl_test = lv_label_create(win, NULL);
	lv_label_set_static_text(lbl_test, lbl_label_colorchange_test);
	lv_obj_set_pos(lbl_test, 40, 140);
	color_akt.label = lbl_test;

	lv_obj_t* line = lv_line_create(win, NULL);
	static const lv_point_t line_points[] = { {0, 0}, { LV_HOR_RES - 80, 0} };
	lv_line_set_points(line, line_points, 2);
	lv_line_set_style(line, lv_theme_get_current()->line.decor);
	lv_obj_align(line, lbl_test, LV_ALIGN_OUT_BOTTOM_LEFT, -10, 20);

	lv_obj_t* lbl_tab_test = lv_label_create(win, NULL);
	lv_label_set_static_text(lbl_tab_test, lbl_tab_label_colorchange_test);
	lv_obj_set_pos(lbl_tab_test, 40, 335);
	color_akt.tab_label = lbl_tab_test;

	lv_obj_t* line2 = lv_line_create(win, NULL);
	lv_line_set_points(line2, line_points, 2);
	lv_line_set_style(line2, lv_theme_get_current()->line.decor);
	lv_obj_align(line2, lbl_tab_test, LV_ALIGN_OUT_BOTTOM_LEFT, -10, 20);

	lv_obj_t* lbl_pl_icon_test_info = lv_label_create(win, NULL);
	lv_label_set_static_text(lbl_pl_icon_test_info, lbl_pl_icon_colorchange_test);
	lv_obj_set_pos(lbl_pl_icon_test_info, 40, 530);
	color_akt.pl_icons_info = lbl_pl_icon_test_info;

	static lv_style_t no_img_label;
	lv_style_copy(&no_img_label, &lv_style_plain);
	no_img_label.text.font = &hekate_symbol_120;

	lv_obj_t* lbl_pl_icon_test = lv_label_create(win, NULL);
	lv_label_set_static_text(lbl_pl_icon_test, SYMBOL_CLOCK);
	lv_obj_align(lbl_pl_icon_test, lbl_pl_icon_test_info, LV_ALIGN_OUT_TOP_RIGHT, 30, 20);
	lv_obj_set_style(lbl_pl_icon_test, &no_img_label);
	color_akt.pl_icons = lbl_pl_icon_test;

	if (color_akt.hue_or_hex_label == 1)
	{
		test_color_label(color_akt.hue);
	}

	else
	{
		test_hex_color_label();
	}

	if (color_akt.hue_or_hex_tab_label == 1)
	{
		test_color_tab(color_akt.hue_tab);
	}

	else
	{
		test_hex_color_tab();
	}

	if (color_akt.hue_or_hex_pl_icon == 1)
	{
		test_color_pl_icon(color_akt.hue_pl_icon);
	}

	else
	{
		test_hex_color_pl_icon();
	}

	return LV_RES_OK;
}


static void load_default_styles(lv_theme_t* th)
{
	if (color_akt.hue_or_hex_label == 1)
	{
		color_label = lv_color_hsv_to_rgb(color_akt.hue, 100, 100);
	}

	else
	{
		color_label = LV_COLOR_MAKE(color_akt.red, color_akt.green, color_akt.blue);
	}

	if (color_akt.hue_or_hex_tab_label == 1)
	{
		color_tab_label = lv_color_hsv_to_rgb(color_akt.hue_tab, 100, 100);
	}

	else
	{
		color_tab_label = LV_COLOR_MAKE(color_akt.red_tab, color_akt.green_tab, color_akt.blue_tab);
	}

	if (color_akt.hue_or_hex_pl_icon == 1)
	{
		color_pl_icon = lv_color_hsv_to_rgb(color_akt.hue_pl_icon, 100, 100);
	}

	else
	{
		color_pl_icon = LV_COLOR_MAKE(color_akt.red_pl_icon, color_akt.green_pl_icon, color_akt.blue_pl_icon);
	}

	lv_style_copy(&mbox_darken, &lv_style_plain);
	mbox_darken.body.main_color = LV_COLOR_BLACK;
	mbox_darken.body.grad_color = mbox_darken.body.main_color;
	mbox_darken.body.opa = LV_OPA_50;
	mbox_darken.body.radius = 5;

	lv_style_copy(&hint_small_style, th->label.hint);
	hint_small_style.text.letter_space = 1;
	hint_small_style.text.font = &interui_20;

	lv_style_copy(&hint_small_style_white, th->label.prim);
	hint_small_style_white.text.letter_space = 1;
	hint_small_style_white.text.font = &interui_20;

	lv_style_copy(&monospace_text, &lv_style_plain);
	monospace_text.body.main_color = LV_COLOR_HEX(0x1B1B1B);
	monospace_text.body.grad_color = LV_COLOR_HEX(0x1B1B1B);
	monospace_text.body.border.color = LV_COLOR_HEX(0x1B1B1B);
	monospace_text.body.border.width = 0;
	monospace_text.body.opa = LV_OPA_TRANSP;
	monospace_text.text.color = LV_COLOR_HEX(0xD8D8D8);
	monospace_text.text.font = &ubuntu_mono;
	monospace_text.text.letter_space = 0;
	monospace_text.text.line_space = 0;

	lv_style_copy(&btn_transp_rel, th->btn.rel);
	btn_transp_rel.body.main_color = LV_COLOR_HEX(0x444444);
	btn_transp_rel.body.grad_color = btn_transp_rel.body.main_color;
	btn_transp_rel.body.opa = LV_OPA_50;

	lv_style_copy(&btn_transp_pr, th->btn.pr);
	btn_transp_pr.body.main_color = LV_COLOR_HEX(0x888888);
	btn_transp_pr.body.grad_color = btn_transp_pr.body.main_color;
	btn_transp_pr.body.opa = LV_OPA_50;

	lv_style_copy(&tabview_btn_pr, th->tabview.btn.pr);
	tabview_btn_pr.body.main_color = LV_COLOR_HEX(0xFFFFFF);
	tabview_btn_pr.body.grad_color = tabview_btn_pr.body.main_color;
	tabview_btn_pr.body.opa = 35;

	lv_style_copy(&tabview_btn_tgl_pr, th->tabview.btn.tgl_pr);
	tabview_btn_tgl_pr.body.main_color = LV_COLOR_HEX(0xFFFFFF);
	tabview_btn_tgl_pr.body.grad_color = tabview_btn_tgl_pr.body.main_color;
	tabview_btn_tgl_pr.body.opa = 35;

	lv_style_copy(&header_style, &lv_style_pretty);
	header_style.text.color = LV_COLOR_WHITE;
	header_style.text.font = &interui_30;
	header_style.body.opa = LV_OPA_50;

	lv_style_copy(&win_bg_style, &lv_style_plain);
	win_bg_style.body.padding.left = LV_DPI / 6;
	win_bg_style.body.padding.right = LV_DPI / 6;
	win_bg_style.body.padding.top = 0;
	win_bg_style.body.padding.bottom = 0;
	win_bg_style.body.padding.inner = LV_DPI / 6;
	win_bg_style.body.main_color = lv_theme_get_current()->bg->body.main_color;
	win_bg_style.body.grad_color = win_bg_style.body.main_color;
	win_bg_style.body.opa = LV_OPA_80;

	lv_style_copy(&style_kb_rel, &lv_style_plain);
	style_kb_rel.body.opa = LV_OPA_TRANSP;
	style_kb_rel.body.radius = 0;
	style_kb_rel.body.border.width = 1;
	style_kb_rel.body.border.color = LV_COLOR_SILVER;
	style_kb_rel.body.border.opa = LV_OPA_50;
	style_kb_rel.body.main_color = LV_COLOR_HEX3(0x333);
	style_kb_rel.body.grad_color = LV_COLOR_HEX3(0x333);
	style_kb_rel.text.color = LV_COLOR_WHITE;

	lv_style_copy(&style_kb_pr, &lv_style_plain);
	style_kb_pr.body.radius = 0;
	style_kb_pr.body.opa = LV_OPA_50;
	style_kb_pr.body.main_color = LV_COLOR_WHITE;
	style_kb_pr.body.grad_color = LV_COLOR_WHITE;
	style_kb_pr.body.border.width = 1;
	style_kb_pr.body.border.color = LV_COLOR_SILVER;

	lv_style_copy(&font20_style, &lv_style_plain);
	font20_style.text.color = color_label;
	font20_style.text.font = &interui_20;

	lv_style_copy(&font20red_style, &lv_style_plain);
	font20red_style.text.color = LV_COLOR_RED;
	font20red_style.text.font = &interui_20;

	lv_style_copy(&font20green_style, &lv_style_plain);
	font20green_style.text.color = LV_COLOR_GREEN;
	font20green_style.text.font = &interui_20;

	lv_style_copy(&labels_style, lv_theme_get_current()->label.prim);
	labels_style.text.color = color_label;

	lv_style_copy(&inv_label, &lv_style_transp);
	inv_label.text.font = NULL;
}

lv_obj_t* gui_create_standard_window(const char* win_title)
{
	lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, win_title);
	lv_win_set_style(win, LV_WIN_STYLE_BG, &win_bg_style);
	lv_win_set_style(win, LV_WIN_STYLE_HEADER, &header_style);
	lv_obj_set_size(win, LV_HOR_RES, LV_VER_RES);
	lv_win_set_btn_size(win, 45);
	close_btn = lv_win_add_btn(win, NULL, SYMBOL_CLOSE" Close", lv_win_close_action_custom);
	return win;
}

lv_res_t lv_win_close_action_firstwin(lv_obj_t* btn)
{
	close_firstwin = NULL;
	return lv_win_close_action(btn);
}

static lv_res_t reboot_to_rcm(lv_obj_t* btn)
{
	if (h_cfg.rcm_patched)
	{
		power_set_state(POWER_OFF_REBOOT);
	}
	else
	{
		power_set_state(REBOOT_RCM);
	}

	return LV_RES_OK;
}

static lv_res_t poweroff(lv_obj_t* btn)
{
	power_set_state(POWER_OFF_RESET);
	return LV_RES_OK;
}

static lv_res_t reboot_ofw(lv_obj_t* btn)
{
	power_set_state(REBOOT_BYPASS_FUSES);
	return LV_RES_OK;
}

static lv_res_t ctrl_info(lv_obj_t* btn)
{
	static lv_style_t bg;
	lv_style_copy(&bg, &lv_style_pretty);
	bg.text.color = color_label;
	bg.body.opa = LV_OPA_0;
	bg.text.font = &interui_20;

	lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
	lv_mbox_set_recolor(mbox, true);
	lv_obj_set_width(mbox, LV_DPI * 5);
	lv_obj_set_top(mbox, true);
	lv_obj_set_auto_realign(mbox, true);
	lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 5);
	lv_mbox_set_text(mbox, mbox_info);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
	lv_mbox_start_auto_close(mbox, 12000);

	return LV_RES_OK;
}

static lv_res_t ctrl_themedel(lv_obj_t* btn)
{
	sd_mount();

	if (f_stat("sxos/titles/0100000000001000", NULL)) {

		static lv_style_t bg;
		lv_style_copy(&bg, &lv_style_pretty);
		bg.text.color = color_label;
		bg.body.opa = LV_OPA_0;
		lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
		lv_mbox_set_recolor(mbox, true);
		lv_obj_set_width(mbox, LV_DPI * 5);
		lv_obj_set_top(mbox, true);
		lv_obj_set_auto_realign(mbox, true);
		lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 5);
		lv_mbox_set_text(mbox, mbox_theme_no);
		lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
		lv_mbox_start_auto_close(mbox, 8000);

	}

	else {

		f_unlink("sxos/titles/0100000000001000");
		f_unlink("sxos/titles/0100000000001007");
		f_unlink("sxos/titles/0100000000001013");
		static lv_style_t bg;
		lv_style_copy(&bg, &lv_style_pretty);
		bg.text.color = LV_COLOR_GREEN;
		bg.body.opa = LV_OPA_0;
		lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
		lv_mbox_set_recolor(mbox, true);
		lv_obj_set_width(mbox, LV_DPI * 5);
		lv_obj_set_top(mbox, true);
		lv_obj_set_auto_realign(mbox, true);
		lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 5);
		lv_mbox_set_text(mbox, mbox_theme_del);
		lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
		lv_mbox_start_auto_close(mbox, 8000);

	}

	sd_unmount(false);
	return LV_RES_OK;
}

static lv_res_t ctrl_themedelATM(lv_obj_t* btn)
{
	sd_mount();

	if (f_stat("atmosphere/contents/0100000000001000", NULL)) {

		static lv_style_t bg;
		lv_style_copy(&bg, &lv_style_pretty);
		bg.text.color = color_label;
		bg.body.opa = LV_OPA_0;
		lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
		lv_mbox_set_recolor(mbox, true);
		lv_obj_set_width(mbox, LV_DPI * 5);
		lv_obj_set_top(mbox, true);
		lv_obj_set_auto_realign(mbox, true);
		lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 5);
		lv_mbox_set_text(mbox, mbox_theme_no);
		lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
		lv_mbox_start_auto_close(mbox, 8000);

	}

	else {

		f_unlink("atmosphere/contents/0100000000001000");
		f_unlink("atmosphere/contents/0100000000001007");
		f_unlink("atmosphere/contents/0100000000001013");
		static lv_style_t bg;
		lv_style_copy(&bg, &lv_style_pretty);
		bg.text.color = LV_COLOR_GREEN;
		bg.body.opa = LV_OPA_0;
		lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
		lv_mbox_set_recolor(mbox, true);
		lv_obj_set_width(mbox, LV_DPI * 5);
		lv_obj_set_top(mbox, true);
		lv_obj_set_auto_realign(mbox, true);
		lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 5);
		lv_mbox_set_text(mbox, mbox_theme_del);
		lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
		lv_mbox_start_auto_close(mbox, 8000);

	}
	sd_unmount(false);
	return LV_RES_OK;
}


static lv_obj_t* kb;

static lv_obj_t* tastd;
static lv_obj_t* tamin;
static lv_obj_t* tatag;
static lv_obj_t* tamon;
static lv_obj_t* tajahr;

static lv_res_t ta_event_action(lv_obj_t* ta)
{
	lv_ta_set_cursor_type(ta, LV_CURSOR_HIDDEN);
	lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);	
	lv_kb_set_ta(kb, ta);

	return LV_RES_OK;
}


static lv_res_t ctrl_rtctimesave(lv_obj_t* btn)
{
	rtc_time_t time;
	const char* getstd = lv_ta_get_text(tastd);
	const char* getmin = lv_ta_get_text(tamin);
	const char* gettag = lv_ta_get_text(tatag);
	const char* getmonth = lv_ta_get_text(tamon);
	const char* getjahr = lv_ta_get_text(tajahr);
	int stdaus = atoi(getstd);
	int minaus = atoi(getmin);
	int tagaus = atoi(gettag);
	int monthaus = atoi(getmonth);
	int jahraus = atoi(getjahr);
	time.hour = stdaus;
	time.min = minaus;
	time.day = tagaus;
	time.month = monthaus;
	time.year = jahraus;

	if (time.hour == 00) time.hour = 23;
	else time.hour -= 1;
	int MEZ = 0;

	if (time.month > 3 && time.month < 10)
	{
		if (time.hour == 00) time.hour = 23;
		else time.hour -= 1;
	}

	if (time.month == 3 && (time.hour + 24 * time.day) >= (1 + MEZ + 24 * (31 - (5 * time.year / 4 + 4) % 7)))
	{
		if (time.hour == 00) time.hour = 23;
		else time.hour -= 1;
	}

	if (time.month == 10 && (time.hour + 24 * time.day) < (1 + MEZ + 24 * (31 - (5 * time.year / 4 + 1) % 7)))
	{
		if (time.hour == 00) time.hour = 23;
		else time.hour -= 1;
	}

	time.year -= 2000;

	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_HOUR_REG, time.hour);
	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_MIN_REG, time.min);
	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_DATE_REG, time.day);
	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_MONTH_REG, time.month);
	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_YEAR_REG, time.year);
	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_UPDATE0_REG, MAX77620_RTC_WRITE_UPDATE);

	static lv_style_t bg;
	lv_style_copy(&bg, &lv_style_pretty);
	bg.text.color = color_label;
	bg.body.opa = LV_OPA_0;
	bg.text.font = &interui_20;
	lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
	lv_mbox_set_recolor(mbox, true);
	lv_obj_set_width(mbox, LV_DPI * 5);
	lv_obj_set_top(mbox, true);
	lv_obj_set_auto_realign(mbox, true);
	lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
	lv_mbox_set_text(mbox, mbox_rtc_time_saved);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
	lv_mbox_start_auto_close(mbox, 8000);
	return LV_RES_OK;
}

static lv_res_t ctrl_rtctime(lv_obj_t* btn)
{
	static lv_style_t tafont110_style;
	lv_style_copy(&tafont110_style, &lv_style_pretty);
	tafont110_style.text.color = LV_COLOR_WHITE;
	tafont110_style.text.font = &num_110;
	tafont110_style.body.opa = LV_OPA_20;

	lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, lbl_win_rtc_time);
	lv_page_set_scrl_layout(lv_win_get_content(win), LV_LAYOUT_OFF);
	lv_win_set_style(win, LV_WIN_STYLE_HEADER, &header_style);
	lv_win_set_style(win, LV_WIN_STYLE_BG, &win_bg_style);

	close_btn = lv_win_add_btn(win, NULL, SYMBOL_CLOSE, lv_win_close_action_custom);
	lv_obj_set_style(close_btn, LV_LABEL_STYLE_MAIN);

	lv_obj_t* save_btn = lv_win_add_btn(win, NULL, SYMBOL_SAVE, ctrl_rtctimesave);
	lv_obj_set_style(save_btn, LV_LABEL_STYLE_MAIN);
	lv_win_set_btn_size(win, 45);

	rtc_time_t time;
	u8 val = 0;

	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_UPDATE0_REG, MAX77620_RTC_READ_UPDATE);
	val = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_CONTROL_REG);
	time.sec = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_SEC_REG) & 0x7F;
	time.min = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_MIN_REG) & 0x7F;
	time.hour = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_HOUR_REG) & 0x1F;

	if (!(val & MAX77620_RTC_24H) && time.hour & MAX77620_RTC_HOUR_PM_MASK)
		time.hour = (time.hour & 0xF) + 12;

	time.weekday = 0;
	val = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_WEEKDAY_REG);
	for (int i = 0; i < 8; i++)
	{
		time.weekday++;
		if (val & 1)
			break;
		val >>= 1;
	}

	time.day = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_DATE_REG) & 0x1f;
	time.year = (i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_YEAR_REG) & 0x7F) + 2000;
	time.month = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_MONTH_REG) & 0xF;

	if (time.hour == 23) time.hour = 0;
	else time.hour += 1;
	int MEZ = 0;

	if (time.month > 3 && time.month < 10)
	{
		if (time.hour == 23) time.hour = 0;
		else time.hour += 1;
	}

	if (time.month == 3 && (time.hour + 24 * time.day) >= (1 + MEZ + 24 * (31 - (5 * time.year / 4 + 4) % 7)))
	{
		if (time.hour == 23) time.hour = 0;
		else time.hour += 1;
	}

	if (time.month == 10 && (time.hour + 24 * time.day) < (1 + MEZ + 24 * (31 - (5 * time.year / 4 + 1) % 7)))
	{
		if (time.hour == 23) time.hour = 0;
		else time.hour += 1;
	}

	char* std = (char*)malloc(64);
	char* min = (char*)malloc(64);
	char* tag = (char*)malloc(64);
	char* mon = (char*)malloc(64);
	char* jahr = (char*)malloc(64);
	s_printf(std, "%02d", time.hour);
	s_printf(min, "%02d", time.min);
	s_printf(tag, "%02d", time.day);
	s_printf(mon, "%02d", time.month);
	s_printf(jahr, "%02d", time.year);

	tastd = lv_ta_create(win, NULL);
	lv_obj_set_size(tastd, 190, 140);
	lv_obj_set_pos(tastd, 25, 50);
	lv_ta_set_text(tastd, std);
	lv_ta_set_max_length(tastd, 2);
	lv_ta_set_style(tastd, LV_LABEL_STYLE_MAIN, &tafont110_style);
	lv_ta_set_cursor_type(tastd, LV_CURSOR_BLOCK);
	lv_ta_set_action(tastd, ta_event_action);

	lv_obj_t* label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_rtc_time_stunde);
	lv_obj_align(label, tastd, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_label_set_style(label, &header_style);

	tamin = lv_ta_create(win, NULL);
	lv_ta_set_cursor_type(tamin, LV_CURSOR_BLOCK | LV_CURSOR_HIDDEN);
	lv_obj_set_size(tamin, 190, 140);
	lv_obj_set_pos(tamin, 235, 16);
	lv_ta_set_text(tamin, min);
	lv_ta_set_max_length(tamin, 2);
	lv_ta_set_style(tamin, LV_LABEL_STYLE_MAIN, &tafont110_style);
	lv_ta_set_action(tamin, ta_event_action);

	label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_rtc_time_minute);
	lv_obj_align(label, tamin, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_label_set_style(label, &header_style);

	tatag = lv_ta_create(win, NULL);
	lv_ta_set_cursor_type(tatag, LV_CURSOR_BLOCK | LV_CURSOR_HIDDEN);
	lv_obj_set_size(tatag, 190, 140);
	lv_obj_set_pos(tatag, 495, 16);
	lv_ta_set_text(tatag, tag);
	lv_ta_set_max_length(tatag, 2);
	lv_ta_set_style(tatag, LV_LABEL_STYLE_MAIN, &tafont110_style);
	lv_ta_set_action(tatag, ta_event_action);

	label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_rtc_time_tag);
	lv_obj_align(label, tatag, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_label_set_style(label, &header_style);

	tamon = lv_ta_create(win, NULL);
	lv_ta_set_cursor_type(tamon, LV_CURSOR_BLOCK | LV_CURSOR_HIDDEN);
	lv_obj_set_size(tamon, 190, 140);
	lv_obj_set_pos(tamon, 705, 16);
	lv_ta_set_text(tamon, mon);
	lv_ta_set_max_length(tamon, 2);
	lv_ta_set_style(tamon, LV_LABEL_STYLE_MAIN, &tafont110_style);
	lv_ta_set_action(tamon, ta_event_action);

	label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_rtc_time_monat);
	lv_obj_align(label, tamon, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_label_set_style(label, &header_style);

	tajahr = lv_ta_create(win, NULL);
	lv_ta_set_cursor_type(tajahr, LV_CURSOR_BLOCK | LV_CURSOR_HIDDEN);
	lv_obj_set_size(tajahr, 330, 140);
	lv_obj_set_pos(tajahr, 915, 16);
	lv_ta_set_text(tajahr, jahr);
	lv_ta_set_max_length(tajahr, 4);
	lv_ta_set_style(tajahr, LV_LABEL_STYLE_MAIN, &tafont110_style);
	lv_ta_set_action(tajahr, ta_event_action);

	label = lv_label_create(win, NULL);
	lv_label_set_text(label, lbl_rtc_time_jahr);
	lv_obj_align(label, tajahr, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_label_set_style(label, &header_style);

	lv_obj_t* label_punkte = lv_label_create(win, NULL);
	lv_label_set_text(label_punkte, ":");
	lv_obj_set_pos(label_punkte, 220, 16);
	lv_label_set_style(label_punkte, &tafont110_style);

	lv_obj_t* label_punkt1 = lv_label_create(win, NULL);
	lv_label_set_text(label_punkt1, ".");
	lv_obj_set_pos(label_punkt1, 690, 16);
	lv_label_set_style(label_punkt1, &tafont110_style);

	lv_obj_t* label_punkt2 = lv_label_create(win, NULL);
	lv_label_set_text(label_punkt2, ".");
	lv_obj_set_pos(label_punkt2, 900, 16);
	lv_label_set_style(label_punkt2, &tafont110_style);

	free(std);
	free(min);
	free(tag);
	free(mon);
	free(jahr);

	kb = lv_kb_create(win, NULL);
	lv_obj_set_size(kb, 1080, 300);
	lv_obj_set_pos(kb, 100, 266);
	lv_kb_set_mode(kb, LV_KB_MODE_NUM);
	lv_kb_set_ta(kb, tastd);
	lv_kb_set_cursor_manage(kb, true);
	lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &style_kb_rel);
	lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &style_kb_pr);
	return LV_RES_OK;
}

//File Manager Start

// Sorry closed Source for now -- You get a part now :)
static lv_obj_t* label_btn;
static lv_obj_t* copybar;
typedef struct _filecopy_prozent_ctxt
{
	u16 copybar_prozent;
	lv_obj_t* label_prozent;
} filecopy_prozent_ctxt;
filecopy_prozent_ctxt filecopy_prozent;

int filecopy_action(const char* pathsourcefile, const char* pathakt)
{
	sd_mount();

#define BUFSIZE 32768
	FIL in, out;
	u64 sizeoffile, sizecopied = 0, totalsize;
	UINT temp1, temp2;
	u8* buff;

	f_open(&in, pathsourcefile, FA_READ | FA_OPEN_EXISTING);
	f_open(&out, pathakt, FA_CREATE_ALWAYS | FA_WRITE);

	buff = malloc(BUFSIZE);
	sizeoffile = f_size(&in);
	totalsize = sizeoffile;

	u32 pct = 0;
	u32 prevPct = 200;

	while (sizeoffile > 0)
	{
		f_read(&in, buff, (sizeoffile > BUFSIZE) ? BUFSIZE : sizeoffile, &temp1);
		f_write(&out, buff, (sizeoffile > BUFSIZE) ? BUFSIZE : sizeoffile, &temp2);

		sizeoffile -= temp1;
		sizecopied += temp1;

		pct = (sizecopied * 100) / totalsize;
		if (pct != prevPct)
		{
			lv_bar_set_value(copybar, pct);
			u16 copybar_read = lv_bar_get_value(copybar);
			filecopy_prozent.copybar_prozent = copybar_read;
			char* prozent = (char*)malloc(64);
			s_printf(prozent, "%d %%", filecopy_prozent.copybar_prozent);
			lv_label_set_array_text(filecopy_prozent.label_prozent, prozent, 64);
			free(prozent);
			lv_refr_now();
			prevPct = pct;
		}
	}
	f_close(&in);
	f_close(&out);
	free(buff);

	sd_unmount(false);

	return 0;
}

static lv_res_t filecopy_cancel(lv_obj_t* btns)
{
	return mbox_action(btns, "Cancel");
}
//Ende Filemanager


//Tasks refresh definitionen	//Nur Texte/Funktionen aktivieren wenn sie auch verwendet werden sonst bootloop
static void update_status(void* params)
{
	char* zeit = (char*)malloc(64);

	rtc_time_t time;

	u8 val = 0;

	i2c_send_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_UPDATE0_REG, MAX77620_RTC_READ_UPDATE);

	val = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_CONTROL_REG);

	time.sec = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_SEC_REG) & 0x7F;
	time.min = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_MIN_REG) & 0x7F;

	time.hour = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_HOUR_REG) & 0x1F;

	if (!(val & MAX77620_RTC_24H) && time.hour & MAX77620_RTC_HOUR_PM_MASK)
		time.hour = (time.hour & 0xF) + 12;

	time.weekday = 0;
	val = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_WEEKDAY_REG);
	for (int i = 0; i < 8; i++)
	{
		time.weekday++;
		if (val & 1)
			break;
		val >>= 1;
	}

	time.day = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_DATE_REG) & 0x1f;
	time.year = (i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_YEAR_REG) & 0x7F) + 2000;
	time.month = i2c_recv_byte(I2C_5, MAX77620_RTC_I2C_ADDR, MAX77620_RTC_MONTH_REG) & 0xF;

	if (time.hour == 23) time.hour = 0;
	else time.hour += 1;
	int MEZ = 0;

	if (time.month > 3 && time.month < 10)
	{
		if (time.hour == 23) time.hour = 0;
		else time.hour += 1;
	}


	if (time.month == 3 && (time.hour + 24 * time.day) >= (1 + MEZ + 24 * (31 - (5 * time.year / 4 + 4) % 7)))
	{
		if (time.hour == 23) time.hour = 0;
		else time.hour += 1;
	}

	if (time.month == 10 && (time.hour + 24 * time.day) < (1 + MEZ + 24 * (31 - (5 * time.year / 4 + 1) % 7)))
	{
		if (time.hour == 23) time.hour = 0;
		else time.hour += 1;
	}

	s_printf(zeit, "%02d.%02d.%02d "" %02d:%02d:%02d",
		time.day, time.month, time.year, time.hour, time.min, time.sec);

	lv_label_set_array_text(status_bar.time_date, zeit, 64);

	free(zeit);

	u32 battPercent = 0;
	max17050_get_property(MAX17050_RepSOC, (int*)&battPercent);

	int per1 = (battPercent >> 8) & 0xFF;
	int per2 = (battPercent & 0xFF) / 25.5001;
	if (per2 >= 0)
		per1 = per1 + 1;

	if (per1 >= 101)
		per1 = per1 - 1;

	if (per1 > 0) {
		lv_label_set_array_text(status_bar.akkusym, SYMBOL_BATTERY_EMPTY, 64);
	}

	if (per1 > 17) {
		lv_label_set_array_text(status_bar.akkusym, SYMBOL_BATTERY_1, 64);
	}

	if (per1 > 34) {
		lv_label_set_array_text(status_bar.akkusym, SYMBOL_BATTERY_1, 64);
	}

	if (per1 > 51) {
		lv_label_set_array_text(status_bar.akkusym, SYMBOL_BATTERY_2, 64);
	}

	if (per1 > 68) {
		lv_label_set_array_text(status_bar.akkusym, SYMBOL_BATTERY_3, 64);
	}

	if (per1 > 84) {
		lv_label_set_array_text(status_bar.akkusym, SYMBOL_BATTERY_FULL, 64);
	}

	if (per1 <= 5) {
		lv_label_set_array_text(status_bar.akkusym, mbox_akku_warnung, 64);
	}


	if (per1 < 20) {

		char* akku = (char*)malloc(0x1000);
		s_printf(akku, "%d %%", per1);
		lv_label_set_array_text(status_bar.akkustand, akku, 64);
		lv_label_set_style(status_bar.akkustand, &font20red_style);
		free(akku);
	}

	else {

		char* akku = (char*)malloc(0x1000);
		s_printf(akku, "%d %%", per1);
		lv_label_set_array_text(status_bar.akkustand, akku, 64);
		lv_label_set_style(status_bar.akkustand, &font20_style);
		free(akku);
	}

	char* amp = (char*)malloc(64);
	char* volt = (char*)malloc(64);
	int batt_volt;
	int batt_curr;

	max17050_get_property(MAX17050_VCELL, &batt_volt);
	max17050_get_property(MAX17050_Current, &batt_curr);

	if (batt_curr >= 0) {

		s_printf(amp, "+%d mA", batt_curr / 1000);
		lv_label_set_array_text(status_bar.battery_more, amp, 64);
		lv_label_set_style(status_bar.battery_more, &font20green_style);
		free(amp);
	}

	else {

		s_printf(amp, "-%d mA", (~batt_curr + 1) / 1000);
		lv_label_set_array_text(status_bar.battery_more, amp, 64);
		lv_label_set_style(status_bar.battery_more, &font20red_style);
		free(amp);
	}

	char* mvolt = (char*)malloc(64);
	s_printf(volt, "%d V", batt_volt);
	s_printf(mvolt, "%d V", batt_volt);
	mvolt[0] = '.';
	volt[strlen(volt) - 5] = '\0';
	strcat(volt, mvolt);

	lv_label_set_array_text(status_bar.battery_more_volt, volt, 64);
	lv_label_set_style(status_bar.battery_more_volt, &font20_style);

	free(volt);
	free(mvolt);

	char* temp = (char*)malloc(64);
	u8 val1;
	u16 soc_temp = 0;

	val1 = i2c_recv_byte(I2C_1, TMP451_I2C_ADDR, TMP451_SOC_TEMP_REG);
	soc_temp = val1 << 8;
	val1 = i2c_recv_byte(I2C_1, TMP451_I2C_ADDR, TMP451_SOC_TMP_DEC_REG);
	soc_temp |= ((val1 >> 4) * 625) / 100;

	s_printf(temp, "CPU %02d.%d", soc_temp >> 8, (soc_temp & 0xFF) / 10);

	lv_label_set_array_text(status_bar.temperatur, temp, 64);
	lv_label_set_style(status_bar.temperatur, &font20_style);

	free(temp);

}

static void create_title(lv_theme_t* th)
{
	lv_obj_t* title = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 65, 620);
	lv_label_set_text(title, "ArgonNX-SE v1.8");
	lv_obj_set_auto_realign(title, true);
	lv_obj_set_style(title, &font20_style);

	static lv_style_t font12_style;
	lv_style_copy(&font12_style, &lv_style_plain);
	font12_style.text.color = LV_COLOR_WHITE;
	font12_style.text.font = &mabolt_12;

	lv_obj_t* lbl_time_temp = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_static_text(lbl_time_temp, "00.00.00 "" 00:00:0000");
	lv_obj_set_pos(lbl_time_temp, 430, 620);
	lv_label_set_style(lbl_time_temp, &font20_style);
	status_bar.time_date = lbl_time_temp;

	lv_obj_t* label_akku = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_pos(label_akku, 350, 620);
	status_bar.akkustand = label_akku;

	lv_obj_t* symb_akku = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_pos(symb_akku, 300, 615);
	lv_label_set_style(symb_akku, &labels_style);
	status_bar.akkusym = symb_akku;

	lv_obj_t* label_strom = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_static_text(label_strom, "+0 mA");
	lv_obj_set_pos(label_strom, 1150, 620);
	status_bar.battery_more = label_strom;

	lv_obj_t* label_volt = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_static_text(label_volt, "0.000 V");
	lv_obj_set_pos(label_volt, 1050, 620);
	status_bar.battery_more_volt = label_volt;

	lv_obj_t* label_temp = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_static_text(label_temp, "CPU 00.0");
	lv_obj_set_pos(label_temp, 900, 620);
	status_bar.temperatur = label_temp;

	lv_obj_t* label_grad = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label_grad, "o");
	lv_obj_set_pos(label_grad, 1000, 620);
	lv_label_set_style(label_grad, &font12_style);

	lv_obj_t* label_c = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label_c, "C");
	lv_obj_set_pos(label_c, 1008, 620);
	lv_label_set_style(label_c, &font20_style);
}

static lv_obj_t* slider;

static lv_res_t ctrl_helligkeit(lv_obj_t* slider)
{
	int slider_hell = lv_slider_get_value(slider);
	display_backlight_brightness(slider_hell, 1000);

	char* hellakt = (char*)malloc(64);
	s_printf(hellakt, "Helligkeit = %d", lv_slider_get_value(slider));

	sd_mount();
	FIL fp;

	f_unlink("argon/sys/hw.ini");

	if (f_stat("argon/sys/hw.ini", NULL)) {

		f_open(&fp, "argon/sys/hw.ini", FA_CREATE_NEW);
		f_close(&fp);

	}

	f_open(&fp, "argon/sys/hw.ini", FA_WRITE);
	f_puts(hellakt, &fp);
	f_close(&fp);

	sd_unmount(false);
	return LV_RES_OK;

}

static int hell;
static lv_obj_t* btn = NULL;
static lv_obj_t* sw_lang;

static lv_res_t ctrl_sw_lang(lv_obj_t* sw_lang)
{
	if (lv_sw_get_state(sw_lang) == false) {

		char* lang = "Sprache = ENG";
		sd_mount();
		FIL fp;

		f_unlink("argon/sys/lang.ini");

		if (f_stat("argon/sys/lang.ini", NULL)) {

			f_open(&fp, "argon/sys/lang.ini", FA_CREATE_NEW);
			f_close(&fp);

		}

		f_open(&fp, "argon/sys/lang.ini", FA_WRITE);
		f_puts(lang, &fp);
		f_close(&fp);

		sd_unmount(false);

		_create_mbox_reload(btn);
	}

	else {

		char* lang = "Sprache = GER";

		sd_mount();
		FIL fp;

		f_unlink("argon/sys/lang.ini");

		if (f_stat("argon/sys/lang.ini", NULL))
		{
			f_open(&fp, "argon/sys/lang.ini", FA_CREATE_NEW);
			f_close(&fp);
		}

		f_open(&fp, "argon/sys/lang.ini", FA_WRITE);
		f_puts(lang, &fp);

		f_close(&fp);

		sd_unmount(false);

		_create_mbox_reload(btn);
	}

	return LV_RES_OK;
}

static lv_obj_t* scr;
static lv_obj_t* tv;

static lv_res_t ctrl_payload_autolaunch(lv_obj_t* list)
{
	const char* filename = lv_list_get_btn_text(list);

	if (!filename || !filename[0])
		goto out;

	char path_source[128];
	char path_dest[128];

	strcpy(path_source, "argon/payloads/");
	strcat(path_source, filename);

	strcpy(path_dest, "argon/");
	strcat(path_dest, filename);

	static lv_style_t mbox_bg;
	lv_style_copy(&mbox_bg, &lv_style_pretty);
	mbox_bg.body.main_color = LV_COLOR_BLACK;
	mbox_bg.body.grad_color = mbox_darken.body.main_color;
	mbox_bg.body.opa = LV_OPA_40;

	lv_obj_t* dark_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_style(dark_bg, &mbox_darken);
	lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

	lv_obj_t* mbox = lv_mbox_create(dark_bg, NULL);
	lv_mbox_set_recolor_text(mbox, true);
	lv_obj_set_width(mbox, 700);

	lv_mbox_set_text(mbox, mbox_fm_filecopy_title);

	static lv_style_t style_bg;
	lv_style_copy(&style_bg, &lv_style_pretty);
	style_bg.body.main_color = LV_COLOR_BLACK;
	style_bg.body.grad_color = LV_COLOR_GRAY;
	style_bg.body.radius = LV_RADIUS_CIRCLE;
	style_bg.body.border.color = LV_COLOR_WHITE;

	static lv_style_t style_indic;
	lv_style_copy(&style_indic, &lv_style_pretty_color);
	style_indic.body.radius = LV_RADIUS_CIRCLE;
	style_indic.body.shadow.width = 8;
	style_indic.body.shadow.color = style_indic.body.main_color;
	style_indic.body.padding.left = 3;
	style_indic.body.padding.right = 3;
	style_indic.body.padding.top = 3;
	style_indic.body.padding.bottom = 3;

	copybar = lv_bar_create(mbox, NULL);
	lv_obj_set_size(copybar, 400, 40);
	lv_bar_set_range(copybar, 0, 100);
	lv_bar_set_value(copybar, 0);
	lv_bar_set_style(copybar, LV_BAR_STYLE_BG, &style_bg);
	lv_bar_set_style(copybar, LV_BAR_STYLE_INDIC, &style_indic);

	lv_obj_t* lbl_prozent = lv_label_create(mbox, NULL);
	lv_label_set_static_text(lbl_prozent, "0 %");
	lv_label_set_style(lbl_prozent, &labels_style);
	filecopy_prozent.label_prozent = lbl_prozent;

	lv_obj_t* btns = lv_btn_create(mbox, NULL);
	label_btn = lv_label_create(btns, NULL);
	lv_label_set_style(label_btn, &inv_label);
	lv_label_set_static_text(label_btn, "Close");
	lv_obj_set_size(btns, 170, 50);
	lv_btn_set_action(btns, LV_BTN_ACTION_CLICK, filecopy_cancel);
	lv_btn_set_style(btns, LV_BTN_STYLE_REL, &inv_label);
	lv_btn_set_style(btns, LV_BTN_STYLE_PR, &inv_label);

	lv_obj_t* label = lv_label_create(mbox, NULL);
	lv_label_set_text(label, path_source);
	lv_label_set_style(label, &font20_style);

	label = lv_label_create(mbox, NULL);
	lv_label_set_text(label, "");
	lv_label_set_style(label, &labels_style);

	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &mbox_bg);
	lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_top(mbox, true);

	manual_system_maintenance(true);

	filecopy_action(path_source, path_dest);
	
	sd_mount();

	f_unlink("argon/autolaunch.bin");
	f_rename(path_dest, "argon/autolaunch.bin");

	sd_unmount();

	lv_bar_set_value(copybar, 100);

	if (filecopy_prozent.copybar_prozent == 100)
	{
		static lv_style_t bg;
		lv_style_copy(&bg, &lv_style_pretty);
		bg.text.color = LV_COLOR_GREEN;
		bg.body.opa = LV_OPA_0;
		bg.text.font = &interui_20;

		lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
		lv_mbox_set_recolor(mbox, true);
		lv_obj_set_width(mbox, LV_DPI * 5);
		lv_obj_set_top(mbox, true);
		lv_obj_set_auto_realign(mbox, true);
		lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
		lv_mbox_set_text(mbox, mbox_autolaunch_set_ok);
		lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
		lv_mbox_start_auto_close(mbox, 12000);

		filecopy_cancel(btns);
	}

out:
	sd_unmount();

	return LV_RES_INV;
}

static lv_res_t reset_autolaunch_action(lv_obj_t* btns, const char* txt)
{
	if (!lv_btnm_get_pressed(btns))
	{
	sd_mount();

	f_unlink("argon/autolaunch.bin");

	sd_unmount();

	static lv_style_t bg;
	lv_style_copy(&bg, &lv_style_pretty);
	bg.text.color = LV_COLOR_GREEN;
	bg.body.opa = LV_OPA_0;
	bg.text.font = &interui_20;

	lv_obj_t* mbox = lv_mbox_create(lv_layer_top(), NULL);
	lv_mbox_set_recolor(mbox, true);
	lv_obj_set_width(mbox, LV_DPI * 5);
	lv_obj_set_top(mbox, true);
	lv_obj_set_auto_realign(mbox, true);
	lv_obj_align(mbox, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
	lv_mbox_set_text(mbox, mbox_autolaunch_reset_ok);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &bg);
	lv_mbox_start_auto_close(mbox, 8000);
	}

	return mbox_action(btns, txt);
}

static lv_res_t mbox_payload_autolaunch(lv_obj_t *btn)
{
	static lv_style_t mbox_bg;
	lv_style_copy(&mbox_bg, &lv_style_pretty);
	mbox_bg.body.main_color = LV_COLOR_BLACK;
	mbox_bg.body.grad_color = mbox_darken.body.main_color;
	mbox_bg.body.opa = LV_OPA_40;
	mbox_bg.text.color = LV_COLOR_WHITE;

	lv_obj_t* dark_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_style(dark_bg, &mbox_darken);
	lv_obj_set_size(dark_bg, LV_HOR_RES, LV_VER_RES);

	static lv_style_t list_style;
	lv_style_copy(&list_style, &lv_style_pretty);
	list_style.text.color = LV_COLOR_WHITE;
	list_style.body.main_color = LV_COLOR_BLACK;
	list_style.body.grad_color = list_style.body.main_color;
	list_style.body.opa = LV_OPA_0;

	static const char* mbox_btn_map[] = { "\221Reset Autolaunch", "\221Cancel", "" };
	lv_obj_t* mbox = lv_mbox_create(dark_bg, NULL);
	lv_mbox_set_recolor_text(mbox, true);

	lv_obj_set_width(mbox, 700);

	lv_mbox_set_text(mbox, mbox_autolaunch_title);

	lv_obj_t *list_pl = lv_list_create(mbox, NULL);
	lv_obj_set_size(list_pl, LV_HOR_RES * 3 / 7, 400);
	lv_list_set_single_mode(list_pl, true);

	lv_list_set_style(list_pl, LV_LIST_STYLE_BG, &list_style);

	if (!sd_mount())
	{
		lv_mbox_set_text(mbox, "#FFDD00 Failed to init SD!#");

		goto out_end;
	}

	char* filelist = dirlist("argon/payloads", "*.bin", false, false);

	sd_unmount();

	u32 i = 0;

	if (filelist)
	{
		while (true)
		{
			if (!filelist[i * 256])
				break;
			lv_list_add(list_pl, NULL, &filelist[i * 256], ctrl_payload_autolaunch);
			i++;
		}
	}

out_end:
	lv_mbox_add_btns(mbox, mbox_btn_map, reset_autolaunch_action);

	lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_top(mbox, true);

	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_BG, &lv_style_transp);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_REL, &btn_transp_rel);
	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BTN_PR, &btn_transp_pr);

	lv_mbox_set_style(mbox, LV_MBOX_STYLE_BG, &mbox_bg);

	return LV_RES_OK;
}

void create_tools_tab(lv_theme_t* th, lv_obj_t* parent)
{
	lv_page_set_scrl_layout(parent, LV_LAYOUT_OFF);
	lv_page_set_scrl_fit(parent, false, false);
	lv_page_set_scrl_height(parent, 620);

	static lv_style_t style_pr;
	lv_style_copy(&style_pr, &lv_style_plain);
	style_pr.image.color = LV_COLOR_BLACK;
	style_pr.image.intense = LV_OPA_50;
	style_pr.text.color = LV_COLOR_HEX3(0xaaa);

	lv_img_dsc_t* img = NULL;

	sd_mount();

	u32 labels_y = 140;

	lv_obj_t* label_sys = lv_label_create(parent, NULL);
	lv_label_set_text(label_sys, SYMBOL_SETTINGS" System Tools");
	lv_obj_set_pos(label_sys, 420, 100);
	lv_label_set_style(label_sys, &labels_style);

	lv_obj_t* btn_sys = lv_cont_create(parent, NULL);
	lv_obj_set_pos(btn_sys, 420, 140);
	lv_obj_set_size(btn_sys, 400, 450);
	lv_cont_set_layout(btn_sys, LV_LAYOUT_COL_L);

	btn = lv_imgbtn_create(btn_sys, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/theme.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, ctrl_themedel);

	lv_obj_t* label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_themedel_sx);
	lv_obj_set_pos(label, 540, 185);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_sys, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/theme.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, ctrl_themedelATM);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_themedel_atm);
	lv_obj_set_pos(label, 540, 300);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_sys, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/fileman.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	//lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, ctrl_filemanager);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Filemanager");
	lv_obj_set_pos(label, 540, 410);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_sys, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/umssd.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, action_ums_sd);

	label = lv_label_create(parent, NULL);
	lv_label_set_recolor(label, true);
	lv_label_set_text(label, "UMS SD Card\n#FF8000 Read/Write.#");
	lv_obj_set_pos(label, 540, 500);
	lv_label_set_style(label, &font20_style);

	lv_obj_t* power_label = lv_label_create(parent, NULL);
	lv_label_set_text(power_label, SYMBOL_POWER" Power Tools");
	lv_obj_set_pos(power_label, 60, 100);
	lv_label_set_style(power_label, &labels_style);

	lv_obj_t* btn_cont = lv_cont_create(parent, NULL);
	lv_obj_set_pos(btn_cont, 60, 140);
	lv_obj_set_size(btn_cont, 350, 450);
	lv_cont_set_layout(btn_cont, LV_LAYOUT_COL_L);

	btn = lv_imgbtn_create(btn_cont, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/power.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, reboot_to_rcm);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Reboot RCM");
	lv_obj_set_pos(label, 180, 185);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_cont, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/power.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, poweroff);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Power off");
	lv_obj_set_pos(label, 180, 300);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_cont, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/power.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, reboot_ofw);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Reboot OFW");
	lv_obj_set_pos(label, 180, 410);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_cont, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/power.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, _create_mbox_reload);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Reload Menu");
	lv_obj_set_pos(label, 180, 520);
	lv_label_set_style(label, &font20_style);

	lv_obj_t* sonst_label = lv_label_create(parent, NULL);
	lv_label_set_text(sonst_label, lbl_title_conf);
	lv_obj_set_pos(sonst_label, 850, labels_y);
	lv_label_set_style(sonst_label, &labels_style);

	lv_obj_t* btn_sonst = lv_cont_create(parent, NULL);
	lv_obj_set_pos(btn_sonst, 850, labels_y + 40);
	lv_obj_set_size(btn_sonst, 400, 450);
	lv_cont_set_layout(btn_sonst, LV_LAYOUT_COL_L);


	btn = lv_imgbtn_create(btn_sonst, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/rtc.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, ctrl_rtctime);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_rtc_time);
	lv_obj_set_pos(label, 970, 220);
	lv_label_set_style(label, &font20_style);


	btn = lv_imgbtn_create(parent, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/colorconf.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_obj_set_pos(btn, 1085, 293);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, ctrl_color_change);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_colorchange);
	lv_obj_set_pos(label, 1195, 325);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(btn_sonst, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/autolaunch.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, mbox_payload_autolaunch);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Autolaunch\nPayload");
	lv_obj_set_pos(label, 965, 325);
	lv_label_set_style(label, &font20_style);

	lv_obj_t* info_label = lv_label_create(parent, NULL);
	lv_label_set_text(info_label, lbl_title_info);
	lv_obj_set_pos(info_label, 850, 420);
	lv_label_set_style(info_label, &labels_style);

	btn = lv_imgbtn_create(parent, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/about.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_obj_set_pos(btn, 1085, 470);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, ctrl_info);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Info");
	lv_obj_set_pos(label, 1200, 510);
	lv_label_set_style(label, &font20_style);

	btn = lv_imgbtn_create(parent, NULL);
	img = bmp_to_lvimg_obj("argon/sys/logos-gui/about.bmp");

	lv_obj_set_size(btn, 100, 100);
	lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
	lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
	lv_obj_set_pos(btn, 855, 470);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, create_win_info);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_cons_info);
	lv_obj_set_pos(label, 970, 500);
	lv_label_set_style(label, &font20_style);

	sd_unmount(false);

	u32 slider_value = hell;

	static lv_style_t style_bg;
	lv_style_copy(&style_bg, &lv_style_pretty);
	style_bg.body.main_color = LV_COLOR_BLACK;
	style_bg.body.grad_color = LV_COLOR_GRAY;
	style_bg.body.radius = LV_RADIUS_CIRCLE;
	style_bg.body.border.color = LV_COLOR_WHITE;

	static lv_style_t style_indic;
	lv_style_copy(&style_indic, &lv_style_pretty_color);
	style_indic.body.radius = LV_RADIUS_CIRCLE;
	style_indic.body.shadow.width = 8;
	style_indic.body.shadow.color = style_indic.body.main_color;
	style_indic.body.padding.left = 3;
	style_indic.body.padding.right = 3;
	style_indic.body.padding.top = 3;
	style_indic.body.padding.bottom = 3;

	static lv_style_t style_knob;
	lv_style_copy(&style_knob, &lv_style_pretty);
	style_knob.body.radius = LV_RADIUS_CIRCLE;
	style_knob.body.opa = LV_OPA_70;
	style_knob.body.padding.top = 10;
	style_knob.body.padding.bottom = 10;

	slider = lv_slider_create(parent, NULL);
	lv_slider_set_style(slider, LV_SLIDER_STYLE_BG, &style_bg);
	lv_slider_set_style(slider, LV_SLIDER_STYLE_INDIC, &style_indic);
	lv_slider_set_style(slider, LV_SLIDER_STYLE_KNOB, &style_knob);
	lv_obj_set_pos(slider, 80, 20);
	lv_obj_set_size(slider, 190, 35);

	lv_slider_set_range(slider, 10, 200);
	lv_slider_set_value(slider, slider_value);
	lv_slider_set_action(slider, ctrl_helligkeit);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_slider_hell);
	lv_obj_set_pos(label, 90, 60);
	lv_label_set_style(label, &font20_style);

	sw_lang = lv_sw_create(parent, NULL);
	lv_sw_set_style(sw_lang, LV_SW_STYLE_BG, &style_bg);
	lv_sw_set_style(sw_lang, LV_SW_STYLE_INDIC, &style_indic);
	lv_sw_set_style(sw_lang, LV_SW_STYLE_KNOB_ON, &style_knob);
	lv_sw_set_style(sw_lang, LV_SW_STYLE_KNOB_OFF, &style_knob);
	lv_obj_set_pos(sw_lang, 500, 20);
	lv_sw_set_anim_time(sw_lang, 250);
	lv_obj_set_size(sw_lang, 150, 35);
	lv_sw_set_action(sw_lang, ctrl_sw_lang);

	if (strcmp(sprache, "GER") == 0)
	{
		lv_sw_off(sw_lang);
	}

	else if (strcmp(sprache, "ENG") == 0)
	{
		lv_sw_on(sw_lang);
	}

	else
	{
		lv_sw_off(sw_lang);
	}

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, lbl_sw_sprache);
	lv_obj_set_pos(label, 470, 60);
	lv_label_set_style(label, &font20_style);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Deutsch");
	lv_obj_set_pos(label, 405, 30);
	lv_label_set_style(label, &font20_style);

	label = lv_label_create(parent, NULL);
	lv_label_set_text(label, "Englisch");
	lv_obj_set_pos(label, 665, 30);
	lv_label_set_style(label, &font20_style);

	lv_obj_t* line = lv_line_create(parent, NULL);
	static lv_point_t line_points[] = { {360, 20}, {360, LV_VER_RES_MAX - 120} };
	lv_line_set_points(line, line_points, 2);
	lv_line_set_style(line, lv_theme_get_current()->line.decor);

	lv_obj_t* line2 = lv_line_create(parent, NULL);
	static lv_point_t line2_points[] = { {790, 20}, {790, LV_VER_RES_MAX - 120} };
	lv_line_set_points(line2, line2_points, 2);
	lv_line_set_style(line2, lv_theme_get_current()->line.decor);
}

void payload_full_path(const char* payload, char* result)
{
	strcpy(result, "argon/payloads");
	strcat(result, "/");
	strcat(result, payload);
}

void payload_logo_path(const char* payload, char* result)
{
	char tmp[256];
	strcpy(tmp, "argon/logos");
	strcat(tmp, "/");
	strcat(tmp, payload);

	strcpy(result, str_replace(tmp, ".bin", ".bmp"));
}

static bool create_payload_entries(lv_theme_t* th, lv_obj_t* parent, char* payloads, u32 group)
{
	lv_obj_t* btn = NULL;
	lv_obj_t* label = NULL;
	lv_img_dsc_t* img = NULL;

	u32 i = 8 * group;

	static lv_style_t style_pr;
	lv_style_copy(&style_pr, &lv_style_plain);
	style_pr.image.color = LV_COLOR_BLACK;
	style_pr.image.intense = LV_OPA_50;
	style_pr.text.color = LV_COLOR_HEX3(0xaaa);

	static lv_style_t no_img_label;
	lv_style_copy(&no_img_label, &lv_style_plain);
	no_img_label.text.font = &hekate_symbol_120;
	no_img_label.text.color = color_pl_icon;

	static lv_style_t no_plimg_label;
	lv_style_copy(&no_plimg_label, &lv_style_plain);
	no_plimg_label.text.font = &interui_20;
	no_plimg_label.text.color = color_pl_icon;


	while (payloads[i * 256] && i < 8 * (group + 1))
	{
		char payload_path[256];
		char payload_logo[256];

		payload_full_path(&payloads[i * 256], payload_path);
		payload_logo_path(&payloads[i * 256], payload_logo);

		img = bmp_to_lvimg_obj((const char*)payload_logo);

		if (!img)
		{
			btn = lv_btn_create(parent, NULL);
			lv_obj_set_size(btn, 280, 280);
			lv_btn_set_style(btn, LV_BTN_STYLE_PR, &btn_transp_pr);
			lv_btn_set_style(btn, LV_BTN_STYLE_REL, &btn_transp_rel);

			label = lv_label_create(btn, NULL);
			lv_obj_set_style(label, &no_img_label);
			lv_label_set_text(label, SYMBOL_CLOCK);

			label = lv_label_create(btn, NULL);
			lv_obj_set_style(label, &no_plimg_label);
			lv_label_set_text(label, &payloads[i * 256]);
		}
		else
		{
			btn = lv_imgbtn_create(parent, NULL);
			lv_imgbtn_set_style(btn, LV_BTN_STATE_PR, &style_pr);
			lv_imgbtn_set_src(btn, LV_BTN_STATE_REL, img);
			lv_imgbtn_set_src(btn, LV_BTN_STATE_PR, img);
		}

		label = lv_label_create(btn, NULL);
		lv_label_set_text(label, payload_path);
		lv_obj_set_style(label, &inv_label);

		lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, launch_payload_btn);

		i++;
	}

	return true;
}

static bool create_tab_payload(lv_theme_t* th, lv_obj_t* par, char* payloads, u32 group, char* tabname)
{
	lv_obj_t* tab_payload = lv_tabview_add_tab(par, tabname);
	lv_page_set_sb_mode(tab_payload, LV_SB_MODE_OFF);

	lv_page_set_scrl_layout(tab_payload, LV_LAYOUT_OFF);
	lv_page_set_scrl_fit(tab_payload, false, false);
	lv_page_set_scrl_height(tab_payload, 620);

	lv_obj_t* page = lv_page_create(tab_payload, NULL);
	lv_obj_set_size(page, lv_obj_get_width(tab_payload), 620);
	lv_obj_align(page, tab_payload, LV_ALIGN_CENTER, 25, 0);
	lv_page_set_scrl_width(page, 0);

	lv_obj_t* plcnr = lv_page_get_scrl(page);
	lv_cont_set_layout(plcnr, LV_LAYOUT_PRETTY);
	lv_obj_set_size(plcnr, LV_HOR_RES_MAX * .95, lv_obj_get_height(page));

	lv_cont_set_style(plcnr, &lv_style_transp);

	create_payload_entries(th, plcnr, payloads, group);

	return true;
}

static bool render_payloads_tab(lv_theme_t* th, lv_obj_t* par)
{

	sd_mount();

	if (f_stat("argon/payloads", NULL)) {

	}

	else {

		char* payloads = dirlist("argon/payloads", "*.bin", false, false);
		u32 i = 0;
		u32 group = 0;

		while (payloads[i * 256])
		{
			if (i % 8 == 0)
			{
				if (group == 2)
				{
					lv_obj_t* label = lv_label_create(lv_tabview_get_tab(tv, 4), NULL);
					lv_label_set_text(label, lbl_pl_more);
					lv_obj_set_style(label, &font20red_style);
					lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

					break;
				}

				if (group == 0)
				{
					create_tab_payload(th, par, payloads, group, SYMBOL_DIRECTORY" Payload");

				}

				else
				{
					create_tab_payload(th, par, payloads, group, SYMBOL_DIRECTORY" Payload2");
				}

				group++;

			}
			i++;
		}
	}
	return true;

	sd_unmount(false);
}


lv_task_t *task_bpmp_clock;
void first_time_bpmp_clock(void *param)
{
	lv_task_del(task_bpmp_clock);
	n_cfg.bpmp_clock = 1;
}

static void load_main_menu(lv_theme_t* th)
{
	c_weiss.red = 0xFF;
	c_weiss.green = 0xFF;
	c_weiss.blue = 0xFF;
	c_schwarz.red = 0x00;
	c_schwarz.green = 0x00;
	c_schwarz.blue = 0x00;
	c_silber.red = 0xC0;
	c_silber.green = 0xC0;
	c_silber.blue = 0xC0;
	c_grau.red = 0x80;
	c_grau.green = 0x80;
	c_grau.blue = 0x80;

	ctrl_color_load(btn);

	load_default_styles(th);

	scr = lv_cont_create(NULL, NULL);
	lv_scr_load(scr);
	lv_cont_set_style(scr, th->bg);

	lv_obj_t* cnr = lv_cont_create(scr, NULL);
	static lv_style_t base_bg_style;
	lv_style_copy(&base_bg_style, &lv_style_plain_color);
	base_bg_style.body.main_color = th->bg->body.main_color;
	base_bg_style.body.grad_color = base_bg_style.body.main_color;
	lv_cont_set_style(cnr, &base_bg_style);
	lv_obj_set_size(cnr, LV_HOR_RES, LV_VER_RES);

	if (hekate_bg)
	{
		lv_obj_t* img = lv_img_create(cnr, NULL);
		lv_img_set_src(img, hekate_bg);
	}

	tv = lv_tabview_create(scr, NULL);
	if (hekate_bg)
	{
		lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_PR, &tabview_btn_pr);
		lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_PR, &tabview_btn_tgl_pr);

		static lv_style_t tabview_btn_rel;
		lv_style_copy(&tabview_btn_rel, th->tabview.btn.bg);
		tabview_btn_rel.text.color = color_tab_label;
		lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_REL, &tabview_btn_rel);
		lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_REL, &tabview_btn_rel);

		lv_tabview_set_btns_pos(tv, LV_TABVIEW_BTNS_POS_BOTTOM);

	}
	lv_tabview_set_sliding(tv, false);
	lv_obj_set_size(tv, LV_HOR_RES, LV_VER_RES);


	sd_mount();
	#define MAXCHAR 100
	FIL fp;
	char info[MAXCHAR];

	if (f_stat("argon/sys/hw.ini", NULL)) {
		display_backlight_brightness(100, 1000);
		hell = 100;
	}

	else {
		f_open(&fp, "argon/sys/hw.ini", FA_READ);
		while (f_gets(info, MAXCHAR, &fp)) {
			char* stelle;
			char* auswahl;
			auswahl = strstr(info, "Helligkeit =");
			stelle = strchr(auswahl, (int)'=');
			infotext = str_replace(stelle, "= ", "");
			hell = atoi(infotext);
			display_backlight_brightness(hell, 1000);
		}
		f_close(&fp);
	}

	if (f_stat("argon/sys/lang.ini", NULL))
	{
		gui_load_lang_ger();
	}

	else {
		f_open(&fp, "argon/sys/lang.ini", FA_READ);
		while (f_gets(info, MAXCHAR, &fp)) {
			char* stelle;
			char* auswahl;
			auswahl = strstr(info, "Sprache =");
			stelle = strchr(auswahl, (int)'=');
			sprache = str_replace(stelle, "= ", "");

			if (strcmp(sprache, "GER") == 0) 
			{
				gui_load_lang_ger();
			}

			else if (strcmp(sprache, "ENG") == 0)
			{
				gui_load_lang_eng();
			}

			else
			{
				lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
				lv_label_set_text(label, "Datei argon/sys/lang.ini ist fehlerhaft!");
				lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
			}

		}
		f_close(&fp);
	}

	sd_unmount(false);

	render_payloads_tab(th, tv);
	lv_obj_t* tab_tools = lv_tabview_add_tab(tv, SYMBOL_TOOLS" Tools");
	create_tools_tab(th, tab_tools);

	create_title(th);

	system_tasks.task.dram_periodic_comp = lv_task_create(minerva_periodic_training, EMC_PERIODIC_TRAIN_MS, LV_TASK_PRIO_HIGHEST, NULL);
	lv_task_ready(system_tasks.task.dram_periodic_comp);

	system_tasks.task.status_bar = lv_task_create(update_status, 500, LV_TASK_PRIO_LOW, NULL);
	lv_task_ready(system_tasks.task.status_bar);

	lv_task_create(_check_sd_card_removed, 2000, LV_TASK_PRIO_LOWEST, NULL);

	task_emmc_errors = lv_task_create(_nyx_emmc_issues, 2000, LV_TASK_PRIO_LOWEST, NULL);
	lv_task_ready(task_emmc_errors);

	if (!n_cfg.bpmp_clock)
		task_bpmp_clock = lv_task_create(first_time_bpmp_clock, 5000, LV_TASK_PRIO_LOWEST, NULL);


}

void gui_load_and_run()
{
	memset(&system_tasks, 0, sizeof(system_maintenance_tasks_t));

	lv_init();
	gfx_con.fillbg = 1;

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.disp_flush = _disp_fb_flush;
	lv_disp_drv_register(&disp_drv);

	lv_task_t* task_jc_init_hw = lv_task_create(jc_init_hw, LV_TASK_ONESHOT, LV_TASK_PRIO_LOWEST, NULL);
	lv_task_once(task_jc_init_hw);
	lv_indev_drv_t indev_drv_jc;
	lv_indev_drv_init(&indev_drv_jc);
	indev_drv_jc.type = LV_INDEV_TYPE_POINTER;
	indev_drv_jc.read = _jc_virt_mouse_read;
	memset(&jc_drv_ctx, 0, sizeof(jc_lv_driver_t));
	jc_drv_ctx.indev = lv_indev_drv_register(&indev_drv_jc);
	close_btn = NULL;

	touch_enabled = touch_power_on();
	lv_indev_drv_t indev_drv_touch;
	lv_indev_drv_init(&indev_drv_touch);
	indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
	indev_drv_touch.read = _fts_touch_read;
	lv_indev_drv_register(&indev_drv_touch);
	touchpad.touch = false;

	tmp451_init();

	lv_theme_t* th = lv_theme_storm_init(0, NULL);
	lv_theme_set_current(th);

	load_main_menu(th);

	jc_drv_ctx.cursor = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(jc_drv_ctx.cursor, &touch_cursor);
	lv_obj_set_opa_scale(jc_drv_ctx.cursor, LV_OPA_TRANSP);
	lv_obj_set_opa_scale_enable(jc_drv_ctx.cursor, true);

	// Check if sd card issues.
	if (sd_get_mode() == SD_1BIT_HS25)
	{
		lv_task_t* task_run_sd_errors = lv_task_create(_nyx_sd_card_issues, LV_TASK_ONESHOT, LV_TASK_PRIO_LOWEST, NULL);
		lv_task_once(task_run_sd_errors);
	}

	// Gui loop.
	if (h_cfg.t210b01)
	{
		// Minerva not supported on T210B01 yet. No power saving.
		while (true)
			lv_task_handler();
	}
	else
	{
		// Alternate DRAM frequencies. Saves 280 mW.
		while (true)
		{
			minerva_change_freq(FREQ_1600);  // Takes 295 us.

			lv_task_handler();

			minerva_change_freq(FREQ_800);   // Takes 80 us.
		}
	}

}
