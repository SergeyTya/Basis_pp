#ifndef LOCALE_H_
#define LOCALE_H


#define LOCALE_RU

#define LABEL_AC1 "AC1"
#define LABEL_AC2 "AC2"
#define LABEL_DC1 "DC1"
#define LABEL_DC2 "DC2"

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
#endif


#ifdef LOCALE_ENG
    #define LG_NAME "12345 1234" // 10
    #define LABEL_7_KATALOG     "CATALOG"   //7
    #define LABEL_8_PARAMETR    "PARAMETR"  //8
    #define LABEL_5_NAZAD       "RETURN"     //5
    #define LABEL_9_SAVE        "SAVE     " //9
    #define LABEL_10_MONIT      "MONITORING"//10
    #define LABEL_9_WARNING     "WARNING  "
    #define LABEL_11_PARAMETERS "PARAMETERS"
    #define LABEL_13_NOTSAVED   "NOT SAVED    "
#endif

#ifdef LOCALE_TRANS
    #define LG_NAME "12345 1234" // 10
    #define LABEL_7_KATALOG     "KATALOG"   //7
    #define LABEL_8_PARAMETR    "PARAMETR"  //8
    #define LABEL_5_NAZAD       "HAZAD"     //5
    #define LABEL_9_SAVE        "COXPAHiTb" //9
    #define LABEL_10_MONIT      "MONITORING"//10
    #define LABEL_9_WARNING     "BHiMAHiE"
    #define LABEL_11_PARAMETERS "PARAMETERy"
    #define LABEL_13_NOTSAVED   "NE COXPAHEHy"
#endif



#endif