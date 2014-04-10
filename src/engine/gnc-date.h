/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
\********************************************************************/
/** @addtogroup Date
    @{ */
/** @file gnc-date.h 
    @brief Date and Time handling routines  
    *
    Utility functions to handle date and time (adjusting, getting 
    the current date, printing the date and time, etc.) 

    \warning HACK ALERT -- the scan and print routines should probably be moved
    to somewhere else. The engine really isn't involved with things
    like printing formats. This is needed mostly by the GUI and so on.
    If a file-io backend needs date handling, it should do it itself,
    instead of depending on the routines here. 

    Overall, this file is quite a mess.  Note, however, that other 
    applications, besides just GnuCash, use this file.  In particular,
    GnoTime (gttr.sourcefore.net) uses this file, and this file is 
    formally a part of QOF (qof.sourceforge.net).
    *
    @author Copyright (C) 1997 Robin D. Clark <rclark@cs.hmc.edu> 
    @author Copyright (C) 1998-2001,2003 Linas Vepstas <linas@linas.org>
*/

#ifndef GNC_DATE_H
#define GNC_DATE_H

#include <glib.h>
#include <time.h>

/** The maximum length of a string created by the date printers */
#define MAX_DATE_LENGTH 11

		/* Deprecated, backwards-compat defines; remove after gnome2 port */
		#define getDateFormatString qof_date_format_get_string
		#define getDateTextFormatString qof_date_format_get_format
		#define getDateFormat qof_date_format_get
		#define setDateFormat qof_date_format_set
		#define DateFormat QofDateFormat
		#define printDateSecs(B,S) qof_print_date_buff(B,MAX_DATE_LENGTH,S)
		#define printDate(B,D,M,Y) qof_print_date_dmy_buff(B,MAX_DATE_LENGTH,D,M,Y)
		#define printGDate(B,D) qof_print_gdate(B,MAX_DATE_LENGTH,D)
		#define xaccPrintDateSecs qof_print_date
		#define scanDate qof_scan_date

		#define DATE_FORMAT_US QOF_DATE_FORMAT_US
  		#define DATE_FORMAT_UK QOF_DATE_FORMAT_UK
  		#define DATE_FORMAT_CE QOF_DATE_FORMAT_CE
  		#define DATE_FORMAT_ISO QOF_DATE_FORMAT_ISO
  		#define DATE_FORMAT_LOCALE QOF_DATE_FORMAT_LOCALE
  		#define DATE_FORMAT_CUSTOM QOF_DATE_FORMAT_CUSTOM

/** Constants *******************************************************/

/** Enum for determining a date format */
typedef enum
{
  QOF_DATE_FORMAT_US,       /**< United states: mm/dd/yyyy */
  QOF_DATE_FORMAT_UK,       /**< Britain: dd/mm/yyyy */
  QOF_DATE_FORMAT_CE,       /**< Continental Europe: dd.mm.yyyy */
  QOF_DATE_FORMAT_ISO,      /**< ISO: yyyy-mm-dd */
  QOF_DATE_FORMAT_LOCALE,   /**< Take from locale information */
  QOF_DATE_FORMAT_CUSTOM    /**< Used by the check printing code */
} QofDateFormat;

#define DATE_FORMAT_FIRST QOF_DATE_FORMAT_US
#define DATE_FORMAT_LAST  QOF_DATE_FORMAT_LOCALE


/**
 * This is how to format the month, as a number, an abbreviated string,
 * or the full name.
 */
typedef enum {
  GNCDATE_MONTH_NUMBER,
  GNCDATE_MONTH_ABBREV,
  GNCDATE_MONTH_NAME
} GNCDateMonthFormat;


/* The string->value versions return 0 on success and 1 on failure */
const char* gnc_date_dateformat_to_string(QofDateFormat format);
gboolean gnc_date_string_to_dateformat(const char* format_string,
				       QofDateFormat *format);


const char* gnc_date_monthformat_to_string(GNCDateMonthFormat format);
gboolean gnc_date_string_to_monthformat(const char *format_string,
					GNCDateMonthFormat *format);


/** Datatypes *******************************************************/

/** struct timespec64 is just like the unix 'struct timespec' except 
 * that we use a 64-bit
 * signed int to store the seconds.  This should adequately cover
 * dates in the distant future as well as the distant past, as long as
 * they're not more than a couple dozen times the age of the universe.
 * Note that both gcc and the IBM Toronto xlC compiler (aka CSet,
 * VisualAge, etc) correctly handle long long as a 64 bit quantity,
 * even on the 32-bit Intel x86 and PowerPC architectures.  I'm
 * assuming that all the other modern compilers are clean on this
 * issue too. */

#ifndef SWIG   /* swig 1.1p5 can't hack the long long type */
struct timespec64
{
   long long int tv_sec;     
   long int tv_nsec;
};
#endif /* SWIG */

/** The Timespec is just like the unix 'struct timespec' 
 * except that we use a 64-bit signed int to
 * store the seconds.  This should adequately cover dates in the
 * distant future as well as the distant past, as long as they're not
 * more than a couple dozen times the age of the universe.  Note that
 * both gcc and the IBM Toronto xlC compiler (aka CSet, VisualAge,
 * etc) correctly handle long long as a 64 bit quantity, even on the
 * 32-bit Intel x86 and PowerPC architectures.  I'm assuming that all
 * the other modern compilers are clean on this issue too. */
typedef struct timespec64 Timespec;


/** Prototypes ******************************************************/

/** @name Timespec functions */
/*@{*/
/** strict equality */
gboolean timespec_equal(const Timespec *ta, const Timespec *tb);

/** comparison:  if (ta < tb) -1; else if (ta > tb) 1; else 0; */
int      timespec_cmp(const Timespec *ta, const Timespec *tb);

/** difference between ta and tb, results are normalised
 * ie tv_sec and tv_nsec of the result have the same size
 * abs(result.tv_nsec) <= 1000000000 */
Timespec timespec_diff(const Timespec *ta, const Timespec *tb);

/** absolute value, also normalised */
Timespec timespec_abs(const Timespec *t);

/** convert a timepair on a certain day (localtime) to
 * the timepair representing midday on that day */
Timespec timespecCanonicalDayTime(Timespec t);

/** Turns a time_t into a Timespec */
void timespecFromTime_t( Timespec *ts, time_t t );

/** Turns a Timespec into a time_t */
time_t timespecToTime_t (Timespec ts);

/** Convert a day, month, and year to a Timespec */
Timespec gnc_dmy2timespec (int day, int month, int year);

/** Same as gnc_dmy2timespec, but last second of the day */
Timespec gnc_dmy2timespec_end (int day, int month, int year);

/** The gnc_iso8601_to_timespec_local() routine converts an ISO-8601 style 
 *    date/time string to Timespec.
 *    For example: 1998-07-17 11:00:00.68-05 
 *    is 680 milliseconds after 11 o'clock, central daylight time 
 *    \return The time in local time.*/
Timespec gnc_iso8601_to_timespec_local(const char *);

/** The gnc_iso8601_to_timespec_gmt() routine converts an ISO-8601 style 
 *    date/time string to Timespec.
 *    For example: 1998-07-17 11:00:00.68-05 
 *    is 680 milliseconds after 11 o'clock, central daylight time 
 *    \return The time in gmt. */
Timespec gnc_iso8601_to_timespec_gmt(const char *);

/** The gnc_timespec_to_iso8601_buff() routine prints a Timespec
* as an ISO-8601 style string.  The buffer must be long enough
* to contain the string.  The string is null-terminated. This
* routine returns a pointer to the null terminator (and can 
* thus be used in the 'stpcpy' metaphor of string concatenation).*/
char * gnc_timespec_to_iso8601_buff (Timespec ts, char * buff);

/** DOCUMENT ME! FIXME: Probably similar to xaccDMYToSec() this date
 * routine might return incorrect values for dates before 1970.  */
void gnc_timespec2dmy (Timespec ts, int *day, int *month, int *year);
/*@}*/


/** Add a number of months to a time value and normalize.  Optionally
 * also track the last day of the month, i.e. 1/31 -> 2/28 -> 3/30. */
void date_add_months (struct tm *tm, int months, gboolean track_last_day);

/** \warning hack alert XXX FIXME -- these date routines return incorrect
 * values for dates before 1970.  Most of them are good only up 
 * till 2038.  This needs fixing ... */
time_t xaccDMYToSec (int day, int month, int year);

/** The gnc_timezone function returns the number of seconds *west*
 * of UTC represented by the tm argument, adjusted for daylight
 * savings time.
 *
 * This function requires a tm argument returned by localtime or set
 * by mktime. This is a strange function! It requires that localtime
 * or mktime be called before use. Subsequent calls to localtime or
 * mktime *may* invalidate the result! The actual contents of tm *may*
 * be used for both timezone offset and daylight savings time, or only
 * daylight savings time! Timezone stuff under unix is not
 * standardized and is a big mess.
 */
long int gnc_timezone (struct tm *tm);


/* ------------------------------------------------------------------------ */
/** @name QofDateFormat functions */
/*@{*/
/** The qof_date_format_get routine returns the date format that
 *  the date printing will use when printing a date, and the scaning 
 *  routines will assume when parsing a date.
 * @returns: the one of the enumerated date formats.
 */
QofDateFormat qof_date_format_get(void);

/** 
 * The qof_date_format_set() routine sets date format to one of
 *    US, UK, CE, OR ISO.  Checks to make sure it's a legal value.
 *    Args: QofDateFormat: enumeration indicating preferred format
 */
void qof_date_format_set(QofDateFormat df);

/** DOCUMENT ME! */
const gchar *qof_date_format_get_string(QofDateFormat df);
/** DOCUMENT ME! */
const gchar *qof_date_format_get_format(QofDateFormat df);
/*@}*/

/** dateSeparator
 *    Return the field separator for the current date format
 *
 * Args:   none
 *
 * Return: date character
 *
 * Globals: global dateFormat value
 */
char dateSeparator(void);

/** @name Date Printing/Scanning functions 
 *
 * \warning HACK ALERT -- the scan and print routines should probably
 * be moved to somewhere else. The engine really isn't involved with
 * things like printing formats. This is needed mostly by the GUI and
 * so on.  If a file-io thing needs date handling, it should do it
 * itself, instead of depending on the routines here.
 */
/*@{*/
/** qof_print_date_dmy_buff
 *    Convert a date as day / month / year integers into a localized string
 *    representation
 *
 * Args:   buff - pointer to previously allocated character array; its size
 *                must be at lease MAX_DATE_LENTH bytes.
 *         len - length of the buffer, in bytes.
 *         day - day of the month as 1 ... 31
 *         month - month of the year as 1 ... 12
 *         year - year (4-digit)
 *
 * Returns: number of characters printed
 *
 * Globals: global dateFormat value
 **/
size_t qof_print_date_dmy_buff (char * buff, size_t buflen, int day, int month, int year);

/** Convenience: calls through to qof_print_date_dmy_buff(). **/
size_t qof_print_date_buff (char * buff, size_t buflen, time_t secs);

/** Convenience; calls through to qof_print_date_dmy_buff(). **/
size_t qof_print_gdate( char *buf, size_t bufflen, GDate *gd );

/** Convenience; calls through to qof_print_date_dmy_buff(). 
 *  Return: string, which should be freed when no longer needed.
 * **/
char * qof_print_date (time_t secs);

/** Convenience; calls through to qof_print_date_dmy_buff(). 
 *  Return: static global string.
 *  \warning This routine is not thread-safe, because it uses a single
 *      global buffer to store the return value.  Use qof_print_date_buff()
 *      or qof_print_date() instead.
 * **/
const char * gnc_print_date(Timespec ts);

/* ------------------------------------------------------------------ */
/* time printing utilities */

/** The qof_print_hours_elapsed_buff() routine will print the 'secs' argument
 *    as HH:MM, and will print the seconds if show_secs is true.  
 *    Thus, for example, secs=3599 will print as 0:59
 *    Returns the number of bytes copied.
 */
size_t qof_print_hours_elapsed_buff (char * buff, size_t len, int secs, gboolean show_secs);
size_t qof_print_minutes_elapsed_buff (char * buff, size_t len, int secs, gboolean show_secs);

/** The qof_print_time_buff() routine prints only the hour-part of the date.
 *    Thus, if secs is  ...
 *    Returns the number of bytes printed.
 */

size_t qof_print_time_buff (char * buff, size_t len, time_t secs);
size_t qof_print_date_time_buff (char * buff, size_t len, time_t secs);

/** The qof_is_same_day() routine returns 0 if both times are in the 
 * same day.
 */

gboolean qof_is_same_day (time_t, time_t);

/* ------------------------------------------------------------------ */
/** The xaccDateUtilGetStamp() routine will take the given time in
 *  seconds and return a buffer containing a textual for the date.
 *  @param thyme The time in seconds to convert.
 *  @return A pointer to the generated string.
 *  @note The caller owns this buffer and must free it when done. */
char * xaccDateUtilGetStamp (time_t thyme);
 
/** qof_scan_date
 *    Convert a string into  day / month / year integers according to
 *    the current dateFormat value.
 *
 * Args:   buff - pointer to date string
 *         day -  will store day of the month as 1 ... 31
 *         month - will store month of the year as 1 ... 12
 *         year - will store the year (4-digit)
 *
 * Return: TRUE if the string seemed to be a valid date; else FALSE.
 *
 * Globals: uses global dateFormat value to assist in parsing.
 */
gboolean qof_scan_date (const char *buff, int *day, int *month, int *year);

/** as above, but returns seconds */
gboolean qof_scan_date_secs (const char *buff, time_t *secs);


/** @name Date Start/End Adjustment routines
 * Given a time value, adjust it to be the beginning or end of that day.
 */
/** @{ */

/** The gnc_tm_set_day_start() inline routine will set the appropriate
 *  fields in the struct tm to indicate the first second of that day.
 *  This routine assumes that the contents of the data structure is
 *  already in normalized form. */
static inline
void gnc_tm_set_day_start (struct tm *tm)
{
  /* First second of the day */
  tm->tm_hour = 0;
  tm->tm_min = 0;
  tm->tm_sec = 0;
  tm->tm_isdst = -1;
}

/** The gnc_tm_set_day_start() inline routine will set the appropriate
 *  fields in the struct tm to indicate noon of that day.  This
 *  routine assumes that the contents of the data structure is already
 *  in normalized form.*/
static inline
void gnc_tm_set_day_middle (struct tm *tm)
{
  /* First second of the day */
  tm->tm_hour = 12;
  tm->tm_min = 0;
  tm->tm_sec = 0;
  tm->tm_isdst = -1;
}

/** The gnc_tm_set_day_start() inline routine will set the appropriate
 *  fields in the struct tm to indicate the last second of that day.
 *  This routine assumes that the contents of the data structure is
 *  already in normalized form.*/
static inline
void gnc_tm_set_day_end (struct tm *tm)
{
  /* Last second of the day */
  tm->tm_hour = 23;
  tm->tm_min = 59;
  tm->tm_sec = 59;
  tm->tm_isdst = -1;
}

/** The gnc_tm_get_day_start() routine will convert the given time in
 *  seconds to the struct tm format, and then adjust it to the
 *  first second of that day. */
void   gnc_tm_get_day_start(struct tm *tm, time_t time_val);

/** The gnc_tm_get_day_end() routine will convert the given time in
 *  seconds to the struct tm format, and then adjust it to the
 *  last second of that day. */
void   gnc_tm_get_day_end(struct tm *tm, time_t time_val);

/** The gnc_timet_get_day_start() routine will take the given time in
 *  seconds and adjust it to the last second of that day. */
time_t gnc_timet_get_day_start(time_t time_val);

/** The gnc_timet_get_day_end() routine will take the given time in
 *  seconds and adjust it to the last second of that day. */
time_t gnc_timet_get_day_end(time_t time_val);

/** The gnc_timet_get_day_start() routine will take the given time in
 *  GLib GDate format and adjust it to the last second of that day. */
time_t gnc_timet_get_day_start_gdate (GDate *date);

/** The gnc_timet_get_day_end() routine will take the given time in
 *  GLib GDate format and adjust it to the last second of that day. */
time_t gnc_timet_get_day_end_gdate (GDate *date);

/** Get the numerical last date of the month. (28, 29, 30, 31) */
int date_get_last_mday(struct tm *tm);

/** Is the mday field the last day of the specified month.*/
gboolean date_is_last_mday(struct tm *tm);

/** DOCUMENT ME! Probably the same as date_get_last_mday() */
int gnc_date_my_last_mday (int month, int year);
/** DOCUMENT ME! Probably the same as date_get_last_mday() */
int gnc_timespec_last_mday (Timespec ts);
/*@}*/

/* ======================================================== */

/** @name Today's Date */
/*@{*/
/** The gnc_tm_get_today_start() routine takes a pointer to a struct
 *  tm and fills it in with the first second of the today. */
void   gnc_tm_get_today_start(struct tm *tm);

/** The gnc_tm_get_today_end() routine takes a pointer to a struct
 *  tm and fills it in with the last second of the today. */
void   gnc_tm_get_today_end(struct tm *tm);

/** The gnc_timet_get_today_start() routine returns a time_t value
 *  corresponding to the first second of today. */
time_t gnc_timet_get_today_start(void);

/** The gnc_timet_get_today_end() routine returns a time_t value
 *  corresponding to the last second of today. */
time_t gnc_timet_get_today_end(void);

/** The xaccDateUtilGetStampNow() routine returns the current time in
 *  seconds in textual format.
 *  @return A pointer to the generated string.
 *  @note The caller owns this buffer and must free it when done. */
char * xaccDateUtilGetStampNow (void);

/*@}*/

#endif /* GNC_DATE_H */
/** @} */