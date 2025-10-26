#ifndef LOCALE_H_
#define LOCALE_H


#define LOCALE_RU

#define LABEL_AC1 "AC "
#define LABEL_AC2 "AC "
#define LABEL_DC1 "DC "
#define LABEL_DC2 "DC "

#ifdef LOCALE_RU
    #define LG_NAME "¡¿«»— ¿›–Œ" // 10
    #define LABEL_7_KATALOG     " ¿“¿ÀŒ√"   //7
    #define LABEL_8_PARAMETR    "œ¿–¿Ã≈“–"  //8
    #define LABEL_5_NAZAD       "Õ¿«¿ƒ"     //5
    #define LABEL_9_SAVE        "—Œ’–¿Õ»“‹" //9
    #define LABEL_10_MONIT      "ÃŒÕ»“Œ–»Õ√"//10
    #define LABEL_9_WARNING     "¬Õ»Ã¿Õ»≈"
    #define LABEL_11_PARAMETERS "œ¿–¿Ã≈“–€"
    #define LABEL_13_NOTSAVED   "Õ≈ «¿œ»—¿ÕÕ€!"
    #define LABEL_AVARIA        "¿¬¿–»ﬂ"
    #define LABEL_YES           "ƒ¿ "
    #define LABEL_NO            "Õ≈“"
#endif


#ifdef LOCALE_ENG
    #define LG_NAME "BAZIS AIR " // 10
    #define LABEL_7_KATALOG     "CATALOG"   //7
    #define LABEL_8_PARAMETR    "PARAMETR"  //8
    #define LABEL_5_NAZAD       "RETURN"     //5
    #define LABEL_9_SAVE        "SAVE     " //9
    #define LABEL_10_MONIT      "MONITORING"//10
    #define LABEL_9_WARNING     "WARNING  "
    #define LABEL_11_PARAMETERS "PARAMETERS"
    #define LABEL_13_NOTSAVED   "NOT SAVED    "
    #define LABEL_AVARIA   "FAULT "
    #define LABEL_YES           "YES"
    #define LABEL_NO            "NO "
#endif

#ifdef LOCALE_TRANS
    #define LG_NAME "BAZIS AERO" // 10
    #define LABEL_7_KATALOG     "KATALOG"   //7
    #define LABEL_8_PARAMETR    "PARAMETR"  //8
    #define LABEL_5_NAZAD       "HAZAD"     //5
    #define LABEL_9_SAVE        "COXPAHiTb" //9
    #define LABEL_10_MONIT      "MONITORING"//10
    #define LABEL_9_WARNING     "BHiMAHiE"
    #define LABEL_11_PARAMETERS "PARAMETERy"
    #define LABEL_13_NOTSAVED   "NE COXPAHEHy"
    #define LABEL_AVARIA   "AVARIA"
    #define LABEL_YES           "D¿ "
    #define LABEL_NO            "HET"
#endif



#endif