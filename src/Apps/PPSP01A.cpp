/*  Compile with ::                                                                    */
/* g++ -shared -fPIC -std=c++11 -Wl,-soname,libPPSP01A.so -o libPPSP01A.so PPSP01A.cpp */

/*
  Copyright (c) 2015 Daniel John Erdos

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/


/* General utilities:                                                       */
/* lspf LOG                                                                 */
/* Application LOG                                                          */
/* PFKEY Settings                                                           */
/* COLOUR Settings                                                          */
/* T0DO list                                                                */
/* SHARED and PROFILE Variable display                                      */
/* Display LIBDEF status and search paths                                   */
/* Display loaded Modules (loaded Dynamic Classes)                          */
/* Display Saved File List                                                  */
/* Simple task list display (from ps aux output)                            */
/* RUN an application (default parameters)                                  */

#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>

#include "../lspf.h"
#include "../utilities.h"
#include "../classes.h"
#include "../pWidgets.h"
#include "../pVPOOL.h"
#include "../pTable.h"
#include "../pPanel.h"
#include "../pApplication.h"

#include "PPSP01A.h"

using namespace std ;
using namespace boost::filesystem ;

#undef MOD_NAME
#undef LOGOUT
#define MOD_NAME PPSP01A
#define LOGOUT aplog


void PPSP01A::application()
{
	log( "I", "Application PPSP01A starting" << endl ) ;

	ZAPPDESC = "General utilities to display logs, PF Key settings, variables, etc." ;

	string LOGTYPE ;
	string ZALOG   ;
	string ZSLOG   ;
	string LOGLOC  ;
	string ZKLUSE  ;
	string w1      ;
	string w2      ;

	w1 = upper( word( PARM, 1 ) ) ;
	w2 = upper( word( PARM, 2 ) ) ;

	vdefine( "ZCMD ZVERB ZROW1 ZROW2 ZAREA ZSHADOW ZAREAT ZSCROLLA", &ZCMD, &ZVERB, &ZROW1, &ZROW2, &ZAREA, &ZSHADOW, &ZAREAT, &ZSCROLLA ) ;
	vdefine( "ZSCROLLN ZAREAW ZAREAD", &ZSCROLLN, &ZAREAW, &ZAREAD ) ;
	vdefine( "ZALOG ZSLOG LOGTYPE LOGLOC ZCOL1", &ZALOG, &ZSLOG, &LOGTYPE, &LOGLOC, &ZCOL1 ) ;

	vget( "ZALOG ZSLOG", PROFILE ) ;

	if ( PARM == "AL" )
	{
		LOGTYPE = "Application" ;
		LOGLOC  = ZALOG   ;
		show_log( ZALOG ) ;
	}
	else if ( PARM == "SL" )
	{
		LOGTYPE = "LSPF"  ;
		LOGLOC  = ZSLOG   ;
		show_log( ZSLOG ) ;
	}
	else if ( PARM == "GOPTS"   ) { lspfSettings()       ; }
	else if ( PARM == "KEYS"    ) { pfkeySettings()      ; }
	else if ( PARM == "COLOURS" ) { colourSettings()     ; }
	else if ( PARM == "TODO"    ) { todoList()           ; }
	else if ( w1   == "VARS"    ) { poolVariables( w2 )  ; }
	else if ( PARM == "PATHS"   ) { showPaths()          ; }
	else if ( PARM == "CMDS"    ) { showCommandTables()  ; }
	else if ( PARM == "MODS"    ) { showLoadedClasses()  ; }
	else if ( w1   == "RUN"     ) { runApplication( w2 ) ; }
	else if ( PARM == "SAVELST" ) { showSavedFileList()  ; }
	else if ( PARM == "TASKS"   ) { showTasks()          ; }
	else if ( PARM == "KLOFF"   )
	{
		vdefine( "ZKLUSE", &ZKLUSE ) ;
		ZKLUSE = "N" ;
		vput( "ZKLUSE", PROFILE ) ;
		vdelete( "ZKLUSE" )       ;
	}
	else if ( PARM == "KLON"   )
	{
		vdefine( "ZKLUSE", &ZKLUSE ) ;
		ZKLUSE = "Y" ;
		vput( "ZKLUSE", PROFILE ) ;
		vdelete( "ZKLUSE" )       ;
	}
	else if ( PARM == "UTPGMS"  ) { utilityPrograms() ; }
	else if ( PARM == "KLISTS"  ) { keylistTables()   ; }
	else if ( PARM == "KLIST"   ) { keylistTable( )   ; }
	else { log( "E", "Invalid parameter passed to PPSP01A: " << PARM << endl ) ; }

	cleanup() ;
	return    ;
}


void PPSP01A::show_log( string fileName )
{
	int fsize ;
	int t     ;

	bool rebuildZAREA ;

	string Rest  ;
	string Restu ;
	string MSG   ;
	string w1    ;
	string w2    ;
	string w3    ;
	string ffilter ;

	filteri = ' '  ;
	filterx = ' '  ;
	ffilter = ""   ;
	firstLine = 0  ;
	maxCol    = 1  ;

	ZAHELP = "HPSP01A" ;

	vget( "ZSCRMAXW ZSCRMAXD", SHARED ) ;
	pquery( "PPSP01AL", "ZAREA", "ZAREAT", "ZAREAW", "ZAREAD" ) ;

	startCol     = 42     ;
	showDate     = false  ;
	showTime     = true   ;
	showMod      = true   ;
	showTask     = true   ;

	read_file( fileName ) ;
	fill_dynamic_area()   ;
	rebuildZAREA = false  ;
	Xon          = false  ;
	MSG          = ""     ;

	fsize = 0  ;
	file_has_changed( fileName, fsize ) ;

	while ( true )
	{
		ZCOL1 = right( d2ds( startCol-41 ), 7, '0' )  ;
		ZROW1 = right( d2ds( firstLine ), 8, '0' )    ;
		ZROW2 = right( d2ds( maxLines ), 8, '0' )     ;
		if ( MSG == "" ) { ZCMD = "" ; }

		display( "PPSP01AL", MSG, "ZCMD" ) ;
		if ( RC == 8 ) { return  ; }
		if ( RC >  8 ) { abend() ; }

		vget( "ZVERB ZSCROLLA ZSCROLLN", SHARED ) ;

		MSG   = "" ;
		Restu = subword( ZCMD, 2 ) ;
		iupper( ( ZCMD ) )  ;
		w1    = word( ZCMD, 1 )    ;
		w2    = word( ZCMD, 2 )    ;
		w3    = word( ZCMD, 3 )    ;
		Rest  = subword( ZCMD, 2 ) ;

		if ( file_has_changed( fileName, fsize ) )
		{
			read_file( fileName ) ;
			if ( ffilter != "" ) { find_lines( ffilter ) ; }
			rebuildZAREA = true ;
			set_excludes() ;
		}

		if ( w1 == "" ) {}
		else if ( w1 == "ONLY" || w1 == "O" )
		{
			if ( Rest.size() == 1 )
			{
				filteri = Rest[ 0 ] ;
				set_excludes() ;
				rebuildZAREA = true ;
			}
			else { MSG = "PPSP012" ; }
		}
		else if ( w1 == "EX" || w1 == "X" )
		{
			if ( Rest == "ALL" )
			{
				exclude_all()   ;
				rebuildZAREA = true ;
			}
			else if ( Rest == "ON" )
			{
				Xon = true     ;
				set_excludes() ;
				rebuildZAREA = true ;
			}
			else if ( Rest.size() == 1 )
			{
				filterx = Rest[ 0 ] ;
				set_excludes() ;
				rebuildZAREA = true ;
			}
			else { MSG = "PPSP013" ; }
		}
		else if ( w1 == "F" || w1 == "FIND" )
		{
			  ffilter = Restu       ;
			  find_lines( ffilter ) ;
			  rebuildZAREA = true   ;
		}
		else if ( abbrev( "RESET", w1 ) )
		{
			filteri = ' '    ;
			filterx = ' '    ;
			ffilter = ""     ;
			Xon     = false  ;
			rebuildZAREA = true ;
			excluded.clear() ;
			for ( t = 0 ; t <= maxLines ; t++ ) { excluded.push_back( false ) ; }
		}
		else if ( abbrev( "REFRESH", w1 ) )
		{
			read_file( fileName ) ;
			rebuildZAREA = true ;
			set_excludes() ;
		}
		else if ( w1 == "SHOW" )
		{
			if      ( w2 == "DATE" ) { showDate = true ; }
			else if ( w2 == "TIME" ) { showTime = true ; }
			else if ( w2 == "MOD"  ) { showMod  = true ; }
			else if ( w2 == "TASK" ) { showTask = true ; }
			else if ( w2 == "ALL"  ) { showDate = true ; showTime = true ; showMod = true ; showTask = true ; }
			rebuildZAREA = true ;
		}
		else if ( w1 == "HIDE" )
		{
			if      ( w2 == "DATE" ) { showDate = false ; }
			else if ( w2 == "TIME" ) { showTime = false ; }
			else if ( w2 == "MOD"  ) { showMod  = false ; }
			else if ( w2 == "TASK" ) { showTask = false ; }
			else if ( w2 == "ALL"  ) { showDate = false ; showTime = false ; showMod = false ; showTask = false ; }
			rebuildZAREA = true ;
		}
		else
		{
			MSG = "PPSP011" ;
			continue        ;
		}

		if ( ZVERB == "DOWN" )
		{
			rebuildZAREA = true ;
			if ( ZSCROLLA == "MAX" )
			{
				firstLine = maxLines - ZAREAD ;
				if ( firstLine < 0 ) firstLine = 0 ;
			}
			else
			{
				t = 0 ;
				for ( ; firstLine < ( maxLines - 1 ) ; firstLine++ )
				{
					if ( excluded[ firstLine ] ) continue ;
					t++ ;
					if ( t > ZSCROLLN ) break ;
				}
			}
		}
		else if ( ZVERB == "UP" )
		{
			rebuildZAREA = true ;
			if ( ZSCROLLA == "MAX" )
			{
				firstLine = 0 ;
			}
			else
			{
				t = 0 ;
				for ( ; firstLine > 0 ; firstLine-- )
				{
					if ( excluded[ firstLine ] ) continue ;
					t++ ;
					if ( t > ZSCROLLN ) break ;
				}
			}
		}
		else if ( ZVERB == "LEFT" )
		{
			rebuildZAREA = true ;
			if ( ZSCROLLA == "MAX" )
			{
				startCol = 42 ;
			}
			else
			{
				startCol = startCol - ZSCROLLN ;
				if ( startCol < 42 ) { startCol = 42 ; }
			}
		}
		else if ( ZVERB == "RIGHT" )
		{
			rebuildZAREA = true ;
			if ( ZSCROLLA == "MAX" )
			{
				startCol = maxCol - ZAREAW + 41 ;
			}
			else
			{
				startCol = startCol + ZSCROLLN ;
			}
		}

		if ( rebuildZAREA ) fill_dynamic_area() ;
		rebuildZAREA = false ;
	}
}


void PPSP01A::read_file( const string& fileName )
{
	string inLine ;
	std::ifstream fin( fileName.c_str() ) ;

	data.clear()     ;

	data.push_back( centre( " Top of Data ", ZAREAW, '*' ) ) ;
	excluded.push_back( false ) ;
	while ( getline( fin, inLine ) )
	{
		inLine = inLine             ;
		data.push_back( inLine )    ;
		excluded.push_back( false ) ;
		if ( maxCol < inLine.size() ) maxCol = inLine.size() ;
	}
	maxCol++   ;
	data.push_back( centre( " Bottom of Data ", ZAREAW, '*' ) ) ;
	excluded.push_back( false ) ;
	maxLines = data.size() ;
	fin.close() ;
}


bool PPSP01A::file_has_changed( const string& fileName, int& fsize )
{
	struct stat results   ;

	lstat( fileName.c_str(), &results ) ;

	if ( fsize == 0 ) { fsize = results.st_size ; }
	else if ( fsize != results.st_size )
	{
		fsize = results.st_size ;
		return true ;
	}
	return false ;
}


void PPSP01A::set_excludes()
{
	int i ;
	for ( i = 1 ; i < maxLines ; i++ )
	{
		if ( Xon )
		{
			if ( data[ i ][ 39 ] == 'D' ||
			     data[ i ][ 39 ] == 'I' ||
			     data[ i ][ 39 ] == '-' )
			{
				excluded[ i ] = true ;
				continue ;
			}
		}
		if ( data[ i ].size() > 39 )
		{
			if ( filteri != ' ' && filteri != data[ i ][ 39 ] && data[ i ][ 39 ] != '*' ) { excluded[ i ] = true ; continue ; }
			if ( filterx != ' ' && filterx == data[ i ][ 39 ] && data[ i ][ 39 ] != '*' ) { excluded[ i ] = true ; continue ; }
		}
	}

}


void PPSP01A::exclude_all()
{
	int i ;
	for ( i = 1 ; i < (maxLines-1) ; i++ ) { excluded[ i ] = true ; }
}


void PPSP01A::find_lines( string fnd )
{
	int i ;

	iupper( fnd ) ;
	for ( i = 1 ; i < (maxLines-1) ; i++ )
	{
		if ( upper( data[ i ] ).find( fnd ) == string::npos ) { excluded[ i ] = true ; }
	}
}


void PPSP01A::fill_dynamic_area()
{
	int p ;

	ZAREA   = "" ;
	ZSHADOW = "" ;
	string s     ;
	string s2( ZAREAW, N_TURQ ) ;
	string t     ;

	int l = 0 ;

	for( unsigned int k = firstLine ; k < data.size() ; k++ )
	{
		if ( excluded[ k ] ) continue ;
		l++ ;
		if ( l > ZAREAD ) break ;
		if ( (k == 0) || (k == maxLines-1 ) )
		{
			ZAREA   = ZAREA + substr( data[ k ], 1, ZAREAW ) ;
			ZSHADOW = ZSHADOW + s2 ;
		}
		else
		{
			t = "" ;
			s = string( ZAREAW, N_GREEN ) ;
			p = 0 ;
			if ( showDate ) { t = data[ k ].substr( 0,  12 )     ; s.replace( p, 12, 12, N_WHITE ) ; p = 12     ; }
			if ( showTime ) { t = t + data[ k ].substr( 12, 16 ) ; s.replace( p, 16, 16, N_WHITE ) ; p = p + 16 ; }
			if ( showMod  ) { t = t + data[ k ].substr( 28, 11 ) ; s.replace( p, 11, 11, N_RED   ) ; p = p + 11 ; }
			t = t + data[ k ].substr( 39, 2 ) ;
			t = t + substr( data[ k ], startCol, ZAREAW ) ;
			t.resize( ZAREAW, ' ' ) ;
			ZAREA = ZAREA + t ;
			s.replace( p, 2, 2, N_TURQ ) ;
			ZSHADOW = ZSHADOW + s ;
		}
	}
}



void PPSP01A::lspfSettings()
{
	int RCode      ;
	int timeOut    ;

	string *t1     ;

	string GODEL   ;
	string GOSWAP  ;
	string GOSWAPC ;
	string GOKLUSE ;
	string GOKLFAL ;
	string GOLMSGW ;

	string ZDEL    ;
	string ZSWAP   ;
	string ZSWAPC  ;
	string ZKLUSE  ;
	string ZKLFAIL ;
	string ZLMSGW  ;

	string GOUCMD1 ;
	string GOUCMD2 ;
	string GOUCMD3 ;
	string GOSCMD1 ;
	string GOSCMD2 ;
	string GOSCMD3 ;
	string GOSTFST ;

	string GOATIMO  ;
	string KMAXWAIT ;

	string GORTSIZE ;
	string GORBSIZE ;
	string ZRTSIZE  ;
	string ZRBSIZE  ;

	string ZUCMDT1 ;
	string ZUCMDT2 ;
	string ZUCMDT3 ;
	string ZSCMDT1 ;
	string ZSCMDT2 ;
	string ZSCMDT3 ;
	string ZSCMDTF ;

	vdefine( "ZUCMDT1 ZUCMDT2 ZUCMDT3", &ZUCMDT1, &ZUCMDT2, &ZUCMDT3 ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "ZSCMDT1 ZSCMDT2 ZSCMDT3 ZSCMDTF", &ZSCMDT1, &ZSCMDT2, &ZSCMDT3, &ZSCMDTF ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "GOUCMD1 GOUCMD2 GOUCMD3", &GOUCMD1, &GOUCMD2, &GOUCMD3 ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "GOSCMD1 GOSCMD2 GOSCMD3 GOSTFST", &GOSCMD1, &GOSCMD2, &GOSCMD3, &GOSTFST ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "ZKLUSE  ZKLFAIL GOKLUSE GOKLFAL", &ZKLUSE, &ZKLFAIL, &GOKLUSE, &GOKLFAL ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "ZDEL    GODEL   ZLMSGW  GOLMSGW", &ZDEL, &GODEL, &ZLMSGW, &GOLMSGW ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "ZSWAP   GOSWAP   ZSWAPC  GOSWAPC",  &ZSWAP, &GOSWAP, &ZSWAPC, &GOSWAPC ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "ZRTSIZE GORTSIZE ZRBSIZE GORBSIZE", &ZRTSIZE, &GORTSIZE, &ZRBSIZE, &GORBSIZE ) ;
	if ( RC > 0 ) { abend() ; }
	vdefine( "ZMAXWAIT GOATIMO", &KMAXWAIT, &GOATIMO ) ;
	if ( RC > 0 ) { abend() ; }

	vget( "ZUCMDT1 ZUCMDT2 ZUCMDT3", PROFILE ) ;
	if ( RC > 0 ) { abend() ; }
	vget( "ZSCMDT1 ZSCMDT2 ZSCMDT3 ZSCMDTF", PROFILE ) ;
	if ( RC > 0 ) { abend() ; }
	vget( "ZSWAP ZSWAPC ZKLUSE ZKLFAIL", PROFILE ) ;
	if ( RC > 0 ) { abend() ; }
	vget( "ZDEL ZKLUSE  ZLMSGW", PROFILE ) ;
	if ( RC > 0 ) { abend() ; }

	if ( ZKLUSE  == "Y" ) { GOKLUSE = "/" ; }
	else                  { GOKLUSE = " " ; }
	if ( ZKLFAIL == "Y" ) { GOKLFAL = "/" ; }
	else                  { GOKLFAL = " " ; }
	if ( ZSCMDTF == "Y" ) { GOSTFST = "/" ; }
	else                  { GOSTFST = " " ; }
	if ( ZLMSGW  == "Y" ) { GOLMSGW = "/" ; }
	else                  { GOLMSGW = " " ; }
	if ( ZSWAP   == "Y" ) { GOSWAP  = "/" ; }
	else                  { GOSWAP  = " " ; }
	GODEL   = ZDEL    ;
	GOSWAPC = ZSWAPC  ;
	GOUCMD1 = ZUCMDT1 ;
	GOUCMD2 = ZUCMDT2 ;
	GOUCMD3 = ZUCMDT3 ;
	GOSCMD1 = ZSCMDT1 ;
	GOSCMD2 = ZSCMDT2 ;
	GOSCMD3 = ZSCMDT3 ;

	vcopy( "ZWAIT", t1, LOCATE ) ;
	vget( "ZMAXWAIT ZRTSIZE ZRBSIZE", PROFILE ) ;

	timeOut = ds2d( *t1 ) * ds2d( KMAXWAIT ) / 1000 ;
	GOATIMO = d2ds( timeOut ) ;

	GORTSIZE = ZRTSIZE ;
	GORBSIZE = ZRBSIZE ;

	while ( true )
	{
		ZCMD  = "" ;
		display( "PPSP01GO", "", "ZCMD" );
		RCode = RC ;
		if ( RCode >  8 ) { abend() ; }
		if ( ZCMD == "CANCEL" ) { break ; }
		if ( RCode == 8 || ZCMD == "SAVE" )
		{
		    if ( GOKLUSE == "/" ) { ZKLUSE  = "Y" ; }
		    else                  { ZKLUSE  = "N" ; }
		    if ( GOKLFAL == "/" ) { ZKLFAIL = "Y" ; }
		    else                  { ZKLFAIL = "N" ; }
		    if ( GOSTFST == "/" ) { ZSCMDTF = "Y" ; }
		    else                  { ZSCMDTF = "N" ; }
		    if ( GOLMSGW == "/" ) { ZLMSGW  = "Y" ; }
		    else                  { ZLMSGW  = "N" ; }
		    if ( GOSWAP  == "/" ) { ZSWAP   = "Y" ; }
		    else                  { ZSWAP   = "N" ; }
		    ZUCMDT1 = GOUCMD1 ;
		    ZUCMDT2 = GOUCMD2 ;
		    ZUCMDT3 = GOUCMD3 ;
		    ZSCMDT1 = GOSCMD1 ;
		    ZSCMDT2 = GOSCMD2 ;
		    ZSCMDT3 = GOSCMD3 ;
		    vput( "ZKLUSE  ZKLFAIL ZLMSGW ZSWAP", PROFILE ) ;
		    vput( "ZUCMDT1 ZUCMDT2 ZUCMDT3", PROFILE ) ;
		    vput( "ZSCMDT1 ZSCMDT2 ZSCMDT3 ZSCMDTF", PROFILE ) ;
		    if ( GODEL != "" && GODEL != ZDEL )
		    {
			    ZDEL = GODEL ;
			    vput( "ZDEL", PROFILE ) ;
		    }
		    if ( GOSWAPC != "" && GOSWAPC != ZSWAPC )
		    {
			    ZSWAPC = GOSWAPC ;
			    vput( "ZSWAPC", PROFILE ) ;
		    }
		    if ( GOATIMO != "" )
		    {
			    KMAXWAIT = d2ds( ds2d( GOATIMO ) * 1000 / ds2d( *t1 ) ) ;
			    vput( "ZMAXWAIT", PROFILE ) ;
		    }
		    if ( GORTSIZE != "" )
		    {
			    ZRTSIZE = GORTSIZE ;
			    vput( "ZRTSIZE", PROFILE ) ;
		    }
		    if ( GORBSIZE != "" )
		    {
			    ZRBSIZE = GORBSIZE ;
			    vput( "ZRBSIZE", PROFILE ) ;
		    }
		    if ( RCode == 8 ) { break ; }
		}
		if ( ZCMD == "DEFAULTS" )
		{
		    GOKLUSE  = ""  ;
		    GOKLFAL  = "/" ;
		    GOSTFST  = "/" ;
		    GOLMSGW  = ""  ;
		    GODEL    = ";" ;
		    GOSWAP   = "/" ;
		    GOSWAPC  = "'" ;
		    GOUCMD1  = "USR" ;
		    GOUCMD2  = ""  ;
		    GOUCMD3  = ""  ;
		    GOSCMD1  = ""  ;
		    GOSCMD2  = ""  ;
		    GOSCMD3  = ""  ;
		    GOATIMO  = d2ds( ZMAXWAIT * ds2d( *t1 ) / 1000 ) ;
		    GORTSIZE = "3"  ;
		    GORBSIZE = "10" ;
		}
	}
	vdelete( "ZUCMDT1 ZUCMDT2 ZUCMDT3" ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "ZSCMDT1 ZSCMDT2 ZSCMDT3 ZSCMDTF" ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "GOUCMD1 GOUCMD2 GOUCMD3" ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "GOSCMD1 GOSCMD2 GOSCMD3 GOSTFST" ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "ZKLUSE  ZKLFAIL GOKLUSE GOKLFAL" ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "ZDEL    GODEL   ZLMSGW  GOLMSGW"  ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "ZSWAP   GOSWAP  ZSWAPC  GOSWAPC"  ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "ZRTSIZE GORTSIZE ZRBSIZE GORBSIZE" ) ;
	if ( RC > 0 ) { abend() ; }
	vdelete( "ZMAXWAIT GOATIMO" ) ;
	if ( RC > 0 ) { abend() ; }
}


void PPSP01A::pfkeySettings()
{
	int RCode ;

	map<int,string> pfKeyDefaults = { {  1, "HELP"      },
					  {  2, "SPLIT NEW" },
					  {  3, "END"       },
					  {  4, "RETURN"    },
					  {  5, "RFIND"     },
					  {  6, "RCHANGE"   },
					  {  7, "UP"        },
					  {  8, "DOWN"      },
					  {  9, "SWAP"      },
					  { 10, "LEFT"      },
					  { 11, "RIGHT"     },
					  { 12, "RETRIEVE"  },
					  { 13, "HELP"      },
					  { 14, "SPLIT NEW" },
					  { 15, "END"       },
					  { 16, "RETURN"    },
					  { 17, "RFIND"     },
					  { 18, "RCHANGE"   },
					  { 19, "UP"        },
					  { 20, "DOWN"      },
					  { 21, "SWAP"      },
					  { 22, "SWAP PREV" },
					  { 23, "SWAP NEXT" },
					  { 24, "HELP"      } } ;

	string ZPF01, ZPF02, ZPF03, ZPF04, ZPF05, ZPF06, ZPF07, ZPF08, ZPF09, ZPF10, ZPF11, ZPF12 ;
	string ZPF13, ZPF14, ZPF15, ZPF16, ZPF17, ZPF18, ZPF19, ZPF20, ZPF21, ZPF22, ZPF23, ZPF24 ;

	vdefine ( "ZPF01 ZPF02 ZPF03 ZPF04 ZPF05 ZPF06 ZPF07 ZPF08", &ZPF01, &ZPF02, &ZPF03, &ZPF04, &ZPF05, &ZPF06, &ZPF07, &ZPF08 ) ;
	vdefine ( "ZPF09 ZPF10 ZPF11 ZPF12 ZPF13 ZPF14 ZPF15 ZPF16", &ZPF09, &ZPF10, &ZPF11, &ZPF12, &ZPF13, &ZPF14, &ZPF15, &ZPF16 ) ;
	vdefine ( "ZPF17 ZPF18 ZPF19 ZPF20 ZPF21 ZPF22 ZPF23 ZPF24", &ZPF17, &ZPF18, &ZPF19, &ZPF20, &ZPF21, &ZPF22, &ZPF23, &ZPF24 ) ;

	vget( "ZPF01 ZPF02 ZPF03 ZPF04 ZPF05 ZPF06 ZPF07 ZPF08 ZPF09 ZPF10 ZPF11 ZPF12", PROFILE ) ;
	vget( "ZPF13 ZPF14 ZPF15 ZPF16 ZPF17 ZPF18 ZPF19 ZPF20 ZPF21 ZPF22 ZPF23 ZPF24", PROFILE ) ;

	while ( true )
	{
		ZCMD  = "" ;
		display( "PPSP01AK", "", "ZCMD" );
		RCode = RC ;
		if (RCode > 8 ) { abend() ; }

		if ( ZCMD == "CANCEL" ) { break ; }
		if ( ZCMD == "DEFAULTS" )
		{
			ZPF01 = "" ;
			ZPF02 = "" ;
			ZPF03 = "" ;
			ZPF04 = "" ;
			ZPF05 = "" ;
			ZPF06 = "" ;
			ZPF07 = "" ;
			ZPF08 = "" ;
			ZPF09 = "" ;
			ZPF10 = "" ;
			ZPF11 = "" ;
			ZPF12 = "" ;
			ZPF13 = "" ;
			ZPF14 = "" ;
			ZPF15 = "" ;
			ZPF16 = "" ;
			ZPF17 = "" ;
			ZPF18 = "" ;
			ZPF19 = "" ;
			ZPF20 = "" ;
			ZPF21 = "" ;
			ZPF22 = "" ;
			ZPF23 = "" ;
			ZPF24 = "" ;
		}

		if ( ZPF01 == "" ) { ZPF01 = pfKeyDefaults[  1 ] ; }
		if ( ZPF02 == "" ) { ZPF02 = pfKeyDefaults[  2 ] ; }
		if ( ZPF03 == "" ) { ZPF03 = pfKeyDefaults[  3 ] ; }
		if ( ZPF04 == "" ) { ZPF04 = pfKeyDefaults[  4 ] ; }
		if ( ZPF05 == "" ) { ZPF05 = pfKeyDefaults[  5 ] ; }
		if ( ZPF06 == "" ) { ZPF06 = pfKeyDefaults[  6 ] ; }
		if ( ZPF07 == "" ) { ZPF07 = pfKeyDefaults[  7 ] ; }
		if ( ZPF08 == "" ) { ZPF08 = pfKeyDefaults[  8 ] ; }
		if ( ZPF09 == "" ) { ZPF09 = pfKeyDefaults[  9 ] ; }
		if ( ZPF10 == "" ) { ZPF10 = pfKeyDefaults[ 10 ] ; }
		if ( ZPF11 == "" ) { ZPF11 = pfKeyDefaults[ 11 ] ; }
		if ( ZPF12 == "" ) { ZPF12 = pfKeyDefaults[ 12 ] ; }
		if ( ZPF13 == "" ) { ZPF13 = pfKeyDefaults[ 13 ] ; }
		if ( ZPF14 == "" ) { ZPF14 = pfKeyDefaults[ 14 ] ; }
		if ( ZPF15 == "" ) { ZPF15 = pfKeyDefaults[ 15 ] ; }
		if ( ZPF16 == "" ) { ZPF16 = pfKeyDefaults[ 16 ] ; }
		if ( ZPF17 == "" ) { ZPF17 = pfKeyDefaults[ 17 ] ; }
		if ( ZPF18 == "" ) { ZPF18 = pfKeyDefaults[ 18 ] ; }
		if ( ZPF19 == "" ) { ZPF19 = pfKeyDefaults[ 19 ] ; }
		if ( ZPF20 == "" ) { ZPF20 = pfKeyDefaults[ 20 ] ; }
		if ( ZPF21 == "" ) { ZPF21 = pfKeyDefaults[ 21 ] ; }
		if ( ZPF22 == "" ) { ZPF22 = pfKeyDefaults[ 22 ] ; }
		if ( ZPF23 == "" ) { ZPF23 = pfKeyDefaults[ 23 ] ; }
		if ( ZPF24 == "" ) { ZPF24 = pfKeyDefaults[ 24 ] ; }

		if ( RCode == 8 || ZCMD == "SAVE" )
		{
			vput( "ZPF01 ZPF02 ZPF03 ZPF04 ZPF05 ZPF06 ZPF07 ZPF08 ZPF09 ZPF10 ZPF11 ZPF12", PROFILE ) ;
			vput( "ZPF13 ZPF14 ZPF15 ZPF16 ZPF17 ZPF18 ZPF19 ZPF20 ZPF21 ZPF22 ZPF23 ZPF24", PROFILE ) ;
			if ( RCode == 8 )  { break ; }
		}
	}
	vdelete( "ZPF01 ZPF02 ZPF03 ZPF04 ZPF05 ZPF06 ZPF07 ZPF08" ) ;
	vdelete( "ZPF09 ZPF10 ZPF11 ZPF12 ZPF13 ZPF14 ZPF15 ZPF16" ) ;
	vdelete( "ZPF17 ZPF18 ZPF19 ZPF20 ZPF21 ZPF22 ZPF23 ZPF24" ) ;
}


void PPSP01A::colourSettings()
{

	int  i ;
	string MSG    ;
	string CURFLD ;
	string var1   ;
	string var2   ;
	string var3   ;
	string val    ;
	string isps_var ;
	string prof_var ;
	string attr1  ;
	string attr2  ;
	string attr3  ;
	string COLOUR ;
	string INTENS ;
	string HILITE ;

	map< int, string>VarList ;
	map< int, string>DefList ;
	map< int, string>OrigList ;

	VarList[ 1  ] = "AB"   ;
	VarList[ 2  ] = "ABSL" ;
	VarList[ 3  ] = "ABU"  ;
	VarList[ 4  ] = "AMT"  ;
	VarList[ 5  ] = "AWF"  ;
	VarList[ 6  ] = "CT"   ;
	VarList[ 7  ] = "CEF"  ;
	VarList[ 8  ] = "CH"   ;
	VarList[ 9  ] = "DT"   ;
	VarList[ 10 ] = "ET"   ;
	VarList[ 11 ] = "EE"   ;
	VarList[ 12 ] = "FP"   ;
	VarList[ 13 ] = "FK"   ;
	VarList[ 14 ] = "IMT"  ;
	VarList[ 15 ] = "LEF"  ;
	VarList[ 16 ] = "LID"  ;
	VarList[ 17 ] = "LI"   ;
	VarList[ 18 ] = "NEF"  ;
	VarList[ 19 ] = "NT"   ;
	VarList[ 20 ] = "PI"   ;
	VarList[ 21 ] = "PIN"  ;
	VarList[ 22 ] = "PT"   ;
	VarList[ 23 ] = "PS"   ;
	VarList[ 24 ] = "PAC"  ;
	VarList[ 25 ] = "PUC"  ;
	VarList[ 26 ] = "RP"   ;
	VarList[ 27 ] = "SI"   ;
	VarList[ 28 ] = "SAC"  ;
	VarList[ 29 ] = "SUC"  ;
	VarList[ 30 ] = "VOI"  ;
	VarList[ 31 ] = "WMT"  ;
	VarList[ 32 ] = "WT"   ;
	VarList[ 33 ] = "WASL" ;

	DefList[ 1  ] = KAB    ;
	DefList[ 2  ] = KABSL  ;
	DefList[ 3  ] = KABU   ;
	DefList[ 4  ] = KAMT   ;
	DefList[ 5  ] = KAWF   ;
	DefList[ 6  ] = KCT    ;
	DefList[ 7  ] = KCEF   ;
	DefList[ 8  ] = KCH    ;
	DefList[ 9  ] = KDT    ;
	DefList[ 10 ] = KET    ;
	DefList[ 11 ] = KEE    ;
	DefList[ 12 ] = KFP    ;
	DefList[ 13 ] = KFK    ;
	DefList[ 14 ] = KIMT   ;
	DefList[ 15 ] = KLEF   ;
	DefList[ 16 ] = KLID   ;
	DefList[ 17 ] = KLI    ;
	DefList[ 18 ] = KNEF   ;
	DefList[ 19 ] = KNT    ;
	DefList[ 20 ] = KPI    ;
	DefList[ 21 ] = KPIN   ;
	DefList[ 22 ] = KPT    ;
	DefList[ 23 ] = KPS    ;
	DefList[ 24 ] = KPAC   ;
	DefList[ 25 ] = KPUC   ;
	DefList[ 26 ] = KRP    ;
	DefList[ 27 ] = KSI    ;
	DefList[ 28 ] = KSAC   ;
	DefList[ 29 ] = KSUC   ;
	DefList[ 30 ] = KVOI   ;
	DefList[ 31 ] = KWMT   ;
	DefList[ 32 ] = KWT    ;
	DefList[ 33 ] = KWASL  ;

	reloadCUATables = true ;

	attr1 = "" ;
	attr2 = "" ;
	attr3 = "" ;

	control( "DISPLAY", "LOCK" ) ;
	display( "PPSP01CL" )        ;

	for ( i = 1 ; i < 34 ; i++ )
	{
		if ( setScreenAttrs( VarList[ i ], i, COLOUR, INTENS, HILITE ) > 0 )
		{
			log( "E", "ISPS variable " << "ZC" + VarList[ i ] << " not found.  Re-run setup program to create" << endl ) ;
			abend() ;
		}
	}

	for ( i = 1 ; i < 34 ; i++ )
	{
		isps_var = "ZC" + VarList[i] ;
		vcopy( isps_var, val, MOVE ) ;
		if ( RC > 0 )
		{
			log( "E", "ISPS variable " << isps_var << " not found.  Re-run setup program to create" << endl ) ;
			abend() ;
		}
		OrigList[ i ] = val ;
	}

	MSG    = "" ;
	ZCMD   = "" ;
	while ( true )
	{
		if ( MSG == "" ) { CURFLD = "ZCMD" ; }
		display( "PPSP01CL", MSG, CURFLD ) ;
		if (RC  > 8 ) { abend()   ;          }
		if (RC == 8 ) { cleanup() ; break  ; }

		if ( ZCMD == "" ) {}
		else if ( ZCMD == "CANCEL" )
		{
			for ( i = 1 ; i < 34 ; i++ )
			{
				isps_var = "ZC" + VarList[ i ] ;
				vcopy( isps_var, val, MOVE )   ;
				if ( RC > 0 )
				{
					log( "E", "ISPS variable " << isps_var << " not found.  Re-run setup program to create" << endl ) ;
					abend() ;
				}
				setISPSVar( VarList[ i ], OrigList[ i ] ) ;
			}
			return ;
		}
		else if ( ZCMD == "DEFAULTS" )
		{
			for ( i = 1 ; i < 34 ; i++ )
			{
				setISPSVar( VarList[ i ], DefList[ i ]  ) ;
			}
			for ( i = 1 ; i < 34 ; i++ )
			{
				setScreenAttrs( VarList[ i ],  i, COLOUR, INTENS, HILITE ) ;
			}
		}
		else if ( ZCMD == "SAVE" )
		{
			ZCMD = "" ;
			for ( i = 1 ; i < 34 ; i++ )
			{
				isps_var = "ZC" + VarList[i] ;
				vcopy( isps_var, val, MOVE ) ;
				if ( RC > 0 )
				{
					log( "E", "ISPS variable " << isps_var << " not found.  Re-run setup program to create" << endl ) ;
					abend() ;
				}
				prof_var      = isps_var  ;
				prof_var[ 0 ] = 'A'       ;
				vdefine( prof_var, &val ) ;
				vput( prof_var, PROFILE ) ;
				vdelete( prof_var ) ;
			}
			if ( RC == 0 ) { MSG = "PPSP011A" ; }
			continue ;
		}
		else if ( ZCMD == "RESTORE" )
		{
			ZCMD = "" ;
			for ( i = 1 ; i < 34 ; i++ )
			{
				prof_var = "AC" + VarList[i] ;
				vcopy( prof_var, val, MOVE ) ;
				if ( RC > 0 ) { MSG = "PPSP019" ; break ; }
				isps_var      = prof_var  ;
				isps_var[ 0 ] = 'Z'       ;
				vdefine( isps_var, &val ) ;
				vput( isps_var, PROFILE ) ;
				vdelete( isps_var ) ;
				if ( setScreenAttrs( VarList[i],  i, COLOUR, INTENS, HILITE ) > 0 ) { abend() ; }
			}
			if ( RC == 0 ) { MSG = "PPSP011B" ; }
			continue ;
		}

		MSG  = "" ;
		ZCMD = "" ;
		for ( i = 1 ; i < 34 ; i++)
		{
			var1 = "COLOUR" + right( d2ds(i), 2, '0') ;
			var2 = "INTENS" + right( d2ds(i), 2, '0') ;
			var3 = "HILITE" + right( d2ds(i), 2, '0') ;
			vcopy( var1, COLOUR, MOVE ) ;
			vcopy( var2, INTENS, MOVE ) ;
			vcopy( var3, HILITE, MOVE ) ;
			if ( COLOUR == "" ) { COLOUR = DefList[ i ][ 0 ] ; }
			if ( INTENS == "" ) { INTENS = DefList[ i ][ 1 ] ; }
			if ( HILITE == "" ) { HILITE = DefList[ i ][ 2 ] ; }
			isps_var = "ZC" + VarList[i] ;
			vcopy( isps_var, val, MOVE ) ;
			if ( RC > 0 )
			{
				log( "E", "ISPS variable " << isps_var << " not found.  Re-run setup program to create" << endl ) ;
				abend() ;
			}
			if ( val.size() != 3 )
			{
				log( "E", "ISPS variable " << isps_var << " has invalid value of " << val << "  Re-run setup program to re-create" << endl ) ;
				abend() ;
			}
			switch ( COLOUR[ 0 ] )
			{
			case 'R':
				vreplace( var1, "RED" ) ;
				val[ 0 ] = 'R'          ;
				attr1 = "COLOUR(RED)"   ;
				break ;
			case 'G':
				vreplace( var1, "GREEN" ) ;
				val[ 0 ] = 'G'            ;
				attr1 = "COLOUR(GREEN)"   ;
				break ;
			case 'Y':
				vreplace( var1, "YELLOW" ) ;
				val[ 0 ] = 'Y'             ;
				attr1 = "COLOUR(YELLOW)"   ;
				break ;
			case 'B':
				vreplace( var1, "BLUE" ) ;
				val[ 0 ] = 'B'           ;
				attr1 = "COLOUR(BLUE)"   ;
				break ;
			case 'M':
				vreplace( var1, "MAGENTA" ) ;
				val[ 0 ] = 'M'              ;
				attr1 = "COLOUR(MAGENTA)"   ;
				break ;
			case 'T':
				vreplace( var1, "TURQ" ) ;
				val[ 0 ] = 'T'           ;
				attr1 = "COLOUR(TURQ)"   ;
				break ;
			case 'W':
				vreplace( var1, "WHITE" ) ;
				val[ 0 ] = 'W'            ;
				attr1 = "COLOUR(WHITE)"   ;
				break ;
			default:
				MSG    = "PPSP016" ;
				CURFLD = var1      ;
			}
			switch ( INTENS[ 0 ] )
			{
			case 'H':
				vreplace( var2, "HIGH" ) ;
				val[ 1 ] = 'H'           ;
				attr2 = "INTENSE(HIGH)"  ;
				break ;
			case 'L':
				vreplace( var2, "LOW"  ) ;
				val[ 1 ] = 'L'           ;
				attr2 = "INTENSE(LOW)"   ;
				break ;
			default:
				MSG    = "PPSP017" ;
				CURFLD = var2      ;
			}
			switch ( HILITE[ 0 ] )
			{
			case 'N':
				vreplace( var3, "NONE" ) ;
				val[ 2 ] = 'N'           ;
				attr3 = "HILITE(NONE)"   ;
				break ;
			case 'B':
				vreplace( var3, "BLINK" ) ;
				val[ 2 ] = 'B'            ;
				attr3 = "HILITE(BLINK)"   ;
					break ;
			case 'R':
				vreplace( var3, "REVERSE" ) ;
				val[ 2 ] = 'R'              ;
				attr3 = "HILITE(REVERSE)"   ;
				break ;
			case 'U':
				vreplace( var3, "USCORE" ) ;
				val[ 2 ] = 'U'             ;
				attr3 = "HILITE(USCORE)"   ;
				break ;
			default:
				MSG    = "PPSP018" ;
				CURFLD = var3      ;
			}
			vdefine( isps_var, &val ) ;
			vput( isps_var, PROFILE ) ;
			vdelete( isps_var )       ;
			attr( var1, attr1 )       ;
			if ( RC > 0 ) { log( "E", "Colour change for field " << var1 << " has failed." << endl ) ; }
			attr( var2, attr1 + " " + attr2 ) ;
			if ( RC > 0 ) { log( "E", "Colour/intense change for field " << var2 << " has failed." << endl ) ; }
			attr( var3, attr1 + " " + attr2 + " " + attr3 ) ;
			if ( RC > 0 ) { log( "E", "Colour/intense/hilite change for field " << var3 << " has failed." << endl ) ; }
		}
	}
}


int PPSP01A::setScreenAttrs( const string& name, int itr, string COLOUR, string INTENS, string HILITE )
{
	string t ;
	char   c ;

	string var1 ;
	string var2 ;
	string var3 ;

	vcopy( "ZC" + name, t, MOVE ) ;
	if ( RC > 0 ) { log( "E", "Variable ZC" << name << " not found in ISPS profile" << endl ) ; return 8 ; }
	else
	{
		if ( t.size() != 3 ) { log( "E", "Variable ZC" << name << " has invalid value " << t << endl ) ; return 8 ; }
		c = t[ 0 ] ;
		if      ( c == 'R' ) COLOUR = "RED"     ;
		else if ( c == 'G' ) COLOUR = "GREEN"   ;
		else if ( c == 'Y' ) COLOUR = "YELLOW"  ;
		else if ( c == 'B' ) COLOUR = "BLUE"    ;
		else if ( c == 'M' ) COLOUR = "MAGENTA" ;
		else if ( c == 'T' ) COLOUR = "TURQ"    ;
		else if ( c == 'W' ) COLOUR = "WHITE"   ;
		else { log( "E", "Variable ZC" << name << " has invalid value " << t << endl ) ; }
		c = t[ 1 ] ;
		if      ( c == 'H' ) INTENS = "HIGH" ;
		else if ( c == 'L' ) INTENS = "LOW" ;
		else { log( "E", "Variable ZC" << name << " has invalid value " << t << endl ) ; }
		c = t[ 2 ] ;
		if      ( c == 'N' ) HILITE = "NONE"    ;
		else if ( c == 'B' ) HILITE = "BLINK"   ;
		else if ( c == 'R' ) HILITE = "REVERSE" ;
		else if ( c == 'U' ) HILITE = "USCORE"  ;
		else { log( "E", "Variable ZC" << name << " has invalid value " << t << endl ) ; }
	}

	var1 = "COLOUR" + right( d2ds( itr ), 2, '0') ;
	var2 = "INTENS" + right( d2ds( itr ), 2, '0') ;
	var3 = "HILITE" + right( d2ds( itr ), 2, '0') ;

	attr( var1, "COLOUR(" + COLOUR + ")" ) ;
	if ( RC > 0 ) { log( "E", "Colour change for field " << var1 << " has failed." << endl ) ; }

	attr( var2, "COLOUR(" + COLOUR + ") INTENSE(" + INTENS + ")" ) ;
	if ( RC > 0 ) { log( "E", "Colour/intense change for field " << var2 << " has failed." << endl ) ; }

	attr( var3,  "COLOUR(" + COLOUR + ") INTENSE(" + INTENS + ") HILITE(" + HILITE + ")" ) ;
	if ( RC > 0 ) { log( "E", "Colour/intense/hilite change for field " << var3 << " has failed." << endl ) ; }

	vreplace( var1, COLOUR ) ;
	vreplace( var2, INTENS ) ;
	vreplace( var3, HILITE ) ;
	return 0 ;
}


void PPSP01A::setISPSVar( const string& var, string val )
{
	string isps_var ;

	isps_var = "ZC" + var     ;
	vdefine( isps_var, &val ) ;
	vput( isps_var, PROFILE ) ;
	vdelete( isps_var )       ;
	return                    ;
}


void PPSP01A::todoList()
{
	// No need to do vdefines for the TODO variables TODO1... as it is not necessary to reference
	// them in the application.  First reference of these variables in the panel will create
	// implicit function pool variables that the vput will save to the PROFILE

	while ( true )
	{
		ZCMD  = "" ;
		display( "PPSP01TD", "", "ZCMD" );
		if (RC  > 8 ) { abend()   ;          }
		if (RC == 8 ) { cleanup() ; break  ; }

		if ( ZCMD == "CANCEL" ) return ;
	}
	vput( "TODO1 TODO2  TODO3  TODO4  TODO5 TODO6 TODO7 TODO8", PROFILE ) ;
	vput( "TODO9 TODO10 TODO11 TODO12", PROFILE ) ;
}


void PPSP01A::poolVariables( const string& applid )
{
	string MSG  ;
	string cw   ;
	string w2   ;

	vcopy( "ZAPPLID", ZAPPLID, MOVE ) ;
	if ( applid != "" && ZAPPLID != applid )
	{
		if ( !isvalidName4( applid ) ) { return ; }
		select( "PGM(PPSP01A) PARM(VARS) NEWAPPL(" + applid + ")" ) ;
		return ;
	}

	VARLST = "VARLST" + right( d2ds( taskid() ), 2, '0' ) ;

	vdefine( "SEL VAR VPOOL VPLVL VAL MESSAGE", &SEL, &VAR, &VPOOL, &VPLVL, &VAL, &MESSAGE ) ;

	getpoolVariables( "" ) ;

	MSG    = "" ;
	ZTDTOP = 1  ;
	while ( true )
	{
		tbtop( VARLST )  ;
		tbskip( VARLST, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD = "" ; }
		tbdispl( VARLST, "PPSP01AV", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		if ( (ZCMD == "REF") || (ZCMD == "RES") ) { tbend( VARLST ) ; getpoolVariables( "" ) ; continue ; }
		cw = word( ZCMD, 1 ) ;
		w2 = word( ZCMD, 2 ) ;
		if ( cw == "O" ) { tbend( VARLST ) ; getpoolVariables( w2 ) ; continue ; }
		if ( ZCMD != "" ) { MSG = "PSYS018" ; continue ; }
		while ( ZTDSELS > 0 )
		{
			if ( SEL == "D" )
			{
				control( "ERRORS", "RETURN" ) ;
				if ( VPOOL == "S" ) verase( VAR, SHARED  ) ;
				else                verase( VAR, PROFILE ) ;
				MESSAGE = "*Delete RC=" + d2ds(RC) + "*" ;
				control( "ERRORS", "CANCEL" ) ;
				SEL = "" ;
			}
			tbput( VARLST ) ;
			if ( ZTDSELS > 1 )
			{
				tbdispl( VARLST ) ;
				if ( RC > 4 ) { break ; }
			}
			else { ZTDSELS = 0 ; }
		}
	}
	tbend( VARLST ) ;
	return ;
}


void PPSP01A::getpoolVariables( const string& pattern )
{
	// SHARED 1  - shared variable pool
	// SHARED 2  - default variable pool (@DEFSHAR)
	// PROFILE 1 - application variable pool
	// PROFILE 2 - Read-only extention pool
	// PROFILE 3 - default read-only profile pool (@DEFPROF)
	// PROFILE 4 - System profile (ISPSPROF)

	string varlist ;
	int i  ;
	int ws ;

	tbcreate( VARLST, "", "SEL VAR VPOOL VPLVL MESSAGE VAL", NOWRITE ) ;

	SEL     = "" ;
	MESSAGE = "" ;

  /*    varlist = vilist( DEFINED ) + vslist( DEFINED ) ;
	VPOOL = "F" ;
	VPLVL = "D" ;
	ws    = words( varlist ) ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	varlist = vilist( IMPLICIT ) + vslist( IMPLICIT ) ;
	VPOOL = "F" ;
	VPLVL = "I" ;
	ws    = words( varlist ) ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}
	*/
	varlist = vlist( SHARED, 1 ) ;
	VPOOL = "S" ;
	VPLVL = "1" ;
	ws    = words( varlist ) ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	varlist = vlist( SHARED, 2 ) ;
	VPLVL   = "2" ;
	ws      = words( varlist ) ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	VPOOL = "P" ;
	VPLVL = "1" ;
	varlist = vlist( PROFILE, 1 ) ;
	ws    = words( varlist ) ;
	for ( i = 1 ; i <= ws ; i++ )
	{

		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	varlist = vlist( PROFILE, 2 ) ;
	VPLVL   = "2"                 ;
	ws      = words( varlist )    ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	varlist = vlist( PROFILE, 3 ) ;
	VPLVL   = "3"                 ;
	ws      = words( varlist )    ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	varlist = vlist( PROFILE, 4 ) ;
	VPLVL   = "4"                 ;
	ws      = words( varlist )    ;
	for ( i = 1 ; i <= ws ; i++ )
	{
		VAR = word( varlist, i ) ;
		if ( (pattern != "") && (pos( pattern, VAR ) == 0) ) { continue ; }
		vcopy( VAR, VAL, MOVE ) ;
		tbadd( VARLST )    ;
	}

	tbtop( VARLST ) ;
}


void PPSP01A::showPaths()
{
	int i ;

	string PGM      ;
	string LIBDEFM  ;
	string LIBDEFP  ;
	string LIBDEFT  ;
	string PATHLST  ;
	string PVAR     ;
	string PATH     ;
	string MESSAGE  ;
	string DESCRIPT ;
	string MSG      ;

	vdefine( "LIBDEFM LIBDEFP LIBDEFT", &LIBDEFM, &LIBDEFP, &LIBDEFT ) ;

	if ( libdef_muser ) { LIBDEFM = "LIBDEF active for user message search"     ; }
	else                { LIBDEFM = "LIBDEF not active for user message search" ; }
	if ( libdef_puser ) { LIBDEFP = "LIBDEF active for user panel search"       ; }
	else                { LIBDEFP = "LIBDEF not active for user panel search"   ; }
	if ( libdef_muser ) { LIBDEFT = "LIBDEF active for user table search"       ; }
	else                { LIBDEFT = "LIBDEF not active for user table search"   ; }

	PATHLST = "PTHLST" + right( d2ds( taskid() ), 2, '0' ) ;

	vdefine( "SEL PVAR PATH MESSAGE DESCRIPT", &SEL, &PVAR, &PATH, &MESSAGE, &DESCRIPT ) ;

	tbcreate( PATHLST, "", "SEL PVAR PATH MESSAGE DESCRIPT", NOWRITE ) ;

	SEL      = ""        ;
	PVAR     = "ZLDPATH" ;
	DESCRIPT = "Path for application modules" ;
	for ( i = 1 ; i <= getpaths( ZLDPATH ) ; i++)
	{
		PATH = getpath( ZLDPATH, i ) ;
		MESSAGE = "" ;
		if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
		tbadd( PATHLST ) ;
		DESCRIPT = "" ;
		PVAR     = "" ;
	}

	if ( libdef_muser )
	{
		PVAR = "ZMUSER" ;
		DESCRIPT = "LIBDEF search path for messages" ;
		for ( i = 1 ; i <= getpaths( ZMUSER ) ; i++)
		{
			PATH = getpath( ZMUSER, i ) ;
			MESSAGE = "" ;
			if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
			tbadd( PATHLST ) ;
			DESCRIPT = "" ;
			PVAR     = "" ;
		}
	}

	PVAR = "ZMLIB" ;
	DESCRIPT = "Search for messages" ;
	for ( i = 1 ; i <= getpaths( ZMLIB ) ; i++)
	{
		PATH = getpath( ZMLIB, i ) ;
		MESSAGE = "" ;
		if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
		tbadd( PATHLST ) ;
		DESCRIPT = "" ;
		PVAR     = "" ;
	}

	if ( libdef_puser )
	{
		PVAR = "ZPUSER" ;
		DESCRIPT = "LIBDEF search path for panels" ;
		for ( i = 1 ; i <= getpaths( ZPUSER ) ; i++)
		{
			PATH = getpath( ZPUSER, i ) ;
			MESSAGE = "" ;
			if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
			tbadd( PATHLST ) ;
			DESCRIPT = "" ;
			PVAR     = "" ;
		}
	}

	PVAR = "ZPLIB" ;
	DESCRIPT = "Search path for panels" ;
	for ( i = 1 ; i <= getpaths( ZPLIB ) ; i++)
	{
		PATH = getpath( ZPLIB, i ) ;
		MESSAGE = "" ;
		if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
		tbadd( PATHLST ) ;
		DESCRIPT = "" ;
		PVAR     = "" ;
	}

	if ( libdef_tuser )
	{
		PVAR = "ZTUSER" ;
		DESCRIPT = "LIBDEF search path for tables" ;
		for ( i = 1 ; i <= getpaths( ZTUSER ) ; i++)
		{
			PATH = getpath( ZTUSER, i ) ;
			MESSAGE = "" ;
			if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
			tbadd( PATHLST ) ;
			DESCRIPT = "" ;
			PVAR     = "" ;
		}
	}

	PVAR     = "ZSPROF" ;
	PATH     =  ZSPROF  ;
	DESCRIPT = "Path for ISPS system profile" ;
	tbadd( PATHLST )    ;

	PVAR     = "ZSYSPATH" ;
	PATH     =  ZSYSPATH  ;
	DESCRIPT = "System Path" ;
	tbadd( PATHLST )    ;

	PVAR = "ZTLIB" ;
	DESCRIPT = "Search path for tables" ;
	for ( i = 1 ; i <= getpaths( ZTLIB ) ; i++)
	{
		PATH = getpath( ZTLIB, i ) ;
		MESSAGE = "" ;
		if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
		tbadd( PATHLST ) ;
		DESCRIPT = "" ;
		PVAR     = "" ;
	}

	PVAR     = "ZUPROF" ;
	PATH     =  ZUPROF  ;
	DESCRIPT = "User home profile path" ;
	tbadd( PATHLST )    ;

	PVAR     = "ZORXPATH" ;
	DESCRIPT = "Object REXX EXEC search path" ;
	for ( i = 1 ; i <= getpaths( ZORXPATH ) ; i++)
	{
		PATH = getpath( ZORXPATH, i ) ;
		MESSAGE = "" ;
		if ( !is_directory( PATH ) ) MESSAGE = "Path not found" ;
		tbadd( PATHLST ) ;
		DESCRIPT = "" ;
		PVAR     = "" ;
	}

	tbtop( PATHLST ) ;
	MSG = "" ;
	ZTDTOP = 1 ;
	while ( true )
	{
		tbtop( PATHLST ) ;
		tbskip( PATHLST, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD  = "" ; }
		tbdispl( PATHLST, "PPSP01AP", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = ""     ;
		while ( ZTDSELS > 0 )
		{
			if ( (SEL == "B") || (SEL == "L") || (SEL == "S"))
			{
				if ( is_directory( PATH ) )
				{
					MESSAGE = "*Listed*" ;
					vcopy( "ZFLSTPGM", PGM, MOVE ) ;
					select( "PGM(" + PGM + ") PARM(" + PATH + ")" ) ;
				}
				else MESSAGE = "*Error*"    ;
			}
			else if ( SEL != "" ) { MESSAGE = "*Error*" ; }
			SEL = ""         ;
			tbput( PATHLST ) ;
			if ( ZTDSELS > 1 )
			{
				tbdispl( PATHLST ) ;
				if ( RC > 4 ) { break ; }
			}
			else { ZTDSELS = 0 ; }
		}
	}
	tbclose( PATHLST ) ;
}


void PPSP01A::showCommandTables()
{
	string CMDTAB   ;
	string OCMDTAB  ;
	string ZCTVERB  ;
	string ZCTTRUNC ;
	string ZCTACT   ;
	string ZCTDESC  ;
	string MSG      ;
	string APPLCMD  ;
	string APPLCMDL ;
	string panel    ;

	vdefine( "ZCTVERB ZCTTRUNC ZCTACT ZCTDESC", &ZCTVERB, &ZCTTRUNC, &ZCTACT, &ZCTDESC ) ;
	vdefine( "ZAPPLID CMDTAB APPLCMD APPLCMDL ZVERB", &ZAPPLID, &CMDTAB, &APPLCMD,  &APPLCMDL, &ZVERB ) ;

	vget( "ZAPPLID" ) ;
	vget( "CMDTAB", PROFILE ) ;
	if ( CMDTAB == "" ) { CMDTAB = "ISP" ; }
	OCMDTAB = CMDTAB ;

	APPLCMD  = "" ;
	APPLCMDL = "" ;
	if ( ZAPPLID != "ISP" )
	{
		APPLCMD = ZAPPLID ;
		tbopen( APPLCMD+"CMDS", NOWRITE, "", SHARE ) ;
		if ( RC > 4 )
		{
			APPLCMDL = "Application Command Table Not Found" ;
		}
		else
		{
			APPLCMDL = "" ;
			tbend( APPLCMD+"CMDS" ) ;
		}
	}
	MSG = "" ;

	tbopen( CMDTAB+"CMDS", NOWRITE, "", SHARE ) ;
	if ( RC > 0 )
	{
		CMDTAB = "ISP" ;
		tbopen( CMDTAB+"CMDS", NOWRITE, "", SHARE ) ;
	}
	ZTDTOP = 1 ;
	panel = "PPSP01AC" ;
	control( "PASSTHRU", "LRSCROLL", "PASON" ) ;
	while ( true )
	{
		tbtop( CMDTAB+"CMDS" ) ;
		tbskip( CMDTAB+"CMDS", ZTDTOP ) ;
		ZCMD  = "" ;
		tbdispl( CMDTAB+"CMDS", panel, MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		if ( OCMDTAB != CMDTAB )
		{
			tbopen( CMDTAB+"CMDS", NOWRITE, "", SHARE ) ;
			if ( RC == 0 )
			{
				tbend( OCMDTAB+"CMDS" ) ;
				OCMDTAB = CMDTAB ;
			}
			else
			{
				CMDTAB = OCMDTAB ;
				MSG = "PPSP014"  ;
			}
		}
		vget( "ZVERB", SHARED ) ;
		if ( ZVERB == "LEFT" || ZVERB == "RIGHT" )
		{
			if ( panel == "PPSP01AC" ) { panel = "PPSP01AD" ; }
			else                       { panel = "PPSP01AC" ; }
		}
	}
	tbend( CMDTAB+"CMDS" ) ;
	vput( "CMDTAB", PROFILE ) ;

}


void PPSP01A::showLoadedClasses()
{
	int j      ;
	int ws     ;

	bool ref   ;

	string w1  ;
	string w2  ;
	string w3  ;
	string MSG ;
	string SEL ;
	string STATUS ;
	string psort  ;

	lspfCommand lc ;

	vdefine( "SEL APPL MOD MODPATH STATUS", &SEL, &APPL, &MOD, &MODPATH, &STATUS ) ;

	MODLST = "MODLST" + right( d2ds( taskid() ), 2, '0' ) ;

	MSG    = ""   ;
	ZTDTOP = 1    ;
	ref    = true ;
	psort  = "APPL,C,A" ;
	while ( true )
	{
		if ( ref )
		{
			tbcreate( MODLST, "APPL", "SEL MOD MODPATH STATUS", NOWRITE ) ;
			tbsort( MODLST, psort ) ;
			lc.Command = "MODULE STATUS" ;
			lspfCallback( lc ) ;
			for ( j = 0 ; j < lc.reply.size() ; j++ )
			{
				SEL     = ""              ;
				APPL    = lc.reply[   j ] ;
				MOD     = lc.reply[ ++j ] ;
				MODPATH = lc.reply[ ++j ] ;
				MODPATH = substr( MODPATH, 1, (lastpos( "/", MODPATH ) - 1) ) ;
				STATUS  = lc.reply[ ++j ] ;
				tbadd( MODLST, "", "ORDER" ) ;
			}
			ref = false ;
		}
		tbtop( MODLST ) ;
		tbskip( MODLST, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD = "" ; }
		tbdispl( MODLST, "PPSP01ML", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		if ( ZTDSELS == 0 && ZCMD == "" ) { ref = true ; }
		ws = words( ZCMD )   ;
		w1 = word( ZCMD, 1 ) ;
		w2 = word( ZCMD, 2 ) ;
		w3 = word( ZCMD, 3 ) ;
		if ( w1 == "SORT" )
		{
			if ( w2 == "" ) { w2 = "APPL" ; }
			if ( w3 == "" ) { w3 = "A"    ; }
			if      ( abbrev( "MODULES", w2, 3 ) )      { psort = "MOD,C,"+w3     ; }
			else if ( abbrev( "APPLICATIONS", w2, 3 ) ) { psort = "APPL,C,"+w3    ; }
			else if ( abbrev( "PATHS", w2, 3 ) )        { psort = "MODPATH,C,"+w3 ; }
			else if ( abbrev( "STATUS", w2, 3 ) )       { psort = "STATUS,C,"+w3  ; }
			else                                        { MSG = "PSYS011C" ; continue ; }
			tbsort( MODLST, psort ) ;
			continue ;
		}
		while ( ZTDSELS > 0 )
		{
			if ( SEL == "R" )
			{
				lc.Command = "MODREL " + APPL ;
				lspfCallback( lc ) ;
			}
			if ( ZTDSELS > 1 )
			{
				tbdispl( MODLST ) ;
				if ( RC > 4 ) { break ; }
			}
			else { ZTDSELS = 0 ; }
		}
		if ( ref ) { tbend( MODLST ) ; }
	}
	tbend( MODLST ) ;
	return ;
}


void PPSP01A::showSavedFileList()
{
	int i ;
	string SEL   ;
	string ZFILE ;
	string ZFILN ;
	string ZCURR ;
	string ZDIR  ;
	string PGM   ;
	string MSG   ;

	vdefine( "ZCURR ZFILE ZDIR", &ZCURR, &ZFILE, &ZDIR ) ;

	MSG = "" ;
	while ( true )
	{
		display( "PPSP01FL", MSG, "ZCMD" ) ;
		if ( RC >  8 ) { abend() ; }
		if ( RC == 8 ) { return  ; }

		if ( ZFILE != "" )
		{
			if ( ZFILE == "*" ) { ZFILE = "" ; }
			if ( ZFILE != "" && ZFILE[ 0 ] == '/' )
			{
				 ZFILN = ZFILE ;
			}
			else if ( ZDIR != "" )
			{
				ZFILN = ZDIR + "/" + ZFILE ;
			}
			else if ( ZCURR != "" )
			{
				ZFILN = ZCURR + "/" + ZFILE ;
			}
			else { continue ; }
			if ( is_directory( ZFILN ) )
			{
				vcopy( "ZFLSTPGM", PGM, MOVE ) ;
				select( "PGM(" + PGM + ") PARM(" + ZFILN + ")" ) ;
			}
			else if ( is_regular_file( ZFILN ) )
			{
				browse( ZFILN ) ;
			}
			continue ;
		}
		for ( i = 1 ; i < 9 ; i++ )
		{
			vcopy( "SEL" + d2ds(i), SEL, MOVE ) ;
			if ( SEL == "" || RC == 8 ) { continue ; }
			vreplace( "SEL" + d2ds(i), "" ) ;
			vcopy( "ZFILE" + d2ds(i), ZFILN, MOVE ) ;
			if ( ZFILN == "" || RC == 8 ) { continue ; }
			if ( (SEL == "S") || (SEL == "L") )
			{
				if ( is_directory( ZFILN ) )
				{
					vcopy( "ZFLSTPGM", PGM, MOVE ) ;
					select( "PGM(" + PGM + ") PARM(" + ZFILN + ")" ) ;
				}
			}
			else if ( SEL == "B" )
			{
				if ( is_regular_file( ZFILN ) )
				{
					browse( ZFILN ) ;
				}
			}
			else if ( SEL == "E" )
			{
				if ( is_regular_file( ZFILN ) )
				{
					edit( ZFILN ) ;
				}
			}
		}
	}
}


void PPSP01A::showTasks()
{
	int retc ;

	string of    ;
	string uf    ;
	string ZCMD  ;
	string SEL   ;
	string USER  ;
	string PID   ;
	string CPU   ;
	string CPUX  ;
	string MEM   ;
	string MEMX  ;
	string CMD   ;
	string MSG   ;
	string TASKLST ;

	MSG = "" ;

	vdefine( "SEL USER PID CPU CPUX MEM MEMX CMD", &SEL, &USER, &PID, &CPU, &CPUX, &MEM, &MEMX, &CMD ) ;

	TASKLST = "TSKLST" + right( d2ds( taskid() ), 2, '0' ) ;

	updateTasks( TASKLST ) ;

	ZTDTOP = 1 ;
	while ( true )
	{
		tbtop( TASKLST ) ;
		tbskip( TASKLST, ZTDTOP ) ;
		ZCMD  = "" ;
		tbdispl( TASKLST, "PPSP01TK", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		vcopy( "USERF", uf, MOVE ) ;
		vcopy( "ONLYF", of, MOVE ) ;
		while ( ZTDSELS > 0 )
		{
			if ( SEL == "K")
			{
				retc = kill( ds2d(PID), 9 ) ;
				log( "I", "Kill signal sent to PID " << PID << ".  RC=" << retc << endl ) ;
			}
			else if ( SEL == "S")
			{
				select( "PGM(PCMD0A) PARM( systemctl status "+ PID +" )" ) ;
			}
			if ( ZTDSELS > 1 )
			{
				tbdispl( TASKLST ) ;
				if ( RC > 4 ) break ;
			}
			else { ZTDSELS = 0 ; }
		}
		tbend( TASKLST ) ;
		updateTasks( TASKLST, uf, of ) ;
	}
	tbend( TASKLST ) ;
	vdelete( "SEL USER PID CPU CPUX MEM MEMX CMD" ) ;
}


void PPSP01A::updateTasks( const string& table, const string& uf, const string& of )
{
	int p ;

	string inLine ;
	string CMD    ;
	string USER   ;
	string CPU    ;
	string CPUX   ;

	std::ifstream fin  ;

	vcopy( "ZUSER", ZUSER, MOVE )     ;
	vcopy( "ZSCREEN", ZSCREEN, MOVE ) ;

	boost::filesystem::path temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path( ZUSER + "-" + ZSCREEN + "-%%%%-%%%%" ) ;
	string tname = temp.native() ;
	string cname = "ps aux > " + tname ;

	tbcreate( table, "", "SEL USER PID CPU CPUX MEM MEMX CMD", NOWRITE ) ;

	system( cname.c_str() ) ;
	fin.open( tname ) ;
	while ( getline( fin, inLine ) )
	{
		tbvclear( table ) ;
		USER = word( inLine, 1 ) ;
		if ( USER == "USER" ) { continue ; }
		if ( uf != "" && uf != upper( USER ) ) { continue ; }
		vreplace( "USER", word( inLine, 1 ) ) ;
		vreplace( "PID", word( inLine, 2 ) ) ;
		CPU  = word( inLine, 3 ) ;
		vreplace( "CPU", CPU ) ;
		p = CPU.find( '.' )      ;
		if ( p == string::npos ) { CPUX = d2ds( ds2d( CPU ) * 10 ) ; }
		else                     { CPUX = CPU ; CPUX.erase( p, 1 ) ; }
		vreplace( "CPUX", CPUX ) ;
		vreplace( "MEM", word( inLine, 4 ) ) ;
		CMD = strip( substr( inLine, 65 ) ) ;
		vreplace( "CMD", CMD ) ;
		if ( of != "" && pos( of, upper( CMD ) ) == 0 ) { continue ; }
		tbadd( table ) ;
	}
	fin.close() ;
	tbsort( table, "CPUX,N,D" ) ;
	tbtop( table ) ;
	remove( tname ) ;
}


void PPSP01A::utilityPrograms()
{

	int RCode ;

	string KMAINPGM ;
	string KPANLPGM ;
	string KEDITPGM ;
	string KBRPGM   ;
	string KVIEWPGM ;
	string KFLSTPGM ;
	string KHELPPGM ;
	string KOREXPGM ;

	string v_list("ZMAINPGM ZPANLPGM ZEDITPGM ZBRPGM ZVIEWPGM ZFLSTPGM ZHELPPGM ZOREXPGM")   ;

	vdefine( v_list, &KMAINPGM, &KPANLPGM, &KEDITPGM, &KBRPGM, &KVIEWPGM, &KFLSTPGM, &KHELPPGM, &KOREXPGM ) ;
	vget( v_list, PROFILE ) ;

	while ( true )
	{
		ZCMD  = "" ;
		display( "PPSP01UP", "", "ZCMD" ) ;
		RCode = RC ;
		if (RCode > 8 ) { abend() ; }

		if ( KMAINPGM == "" ) { KMAINPGM = ZMAINPGM ; } ;
		if ( KPANLPGM == "" ) { KPANLPGM = ZPANLPGM ; } ;
		if ( KEDITPGM == "" ) { KEDITPGM = ZEDITPGM ; } ;
		if ( KBRPGM   == "" ) { KBRPGM   = ZBRPGM   ; } ;
		if ( KVIEWPGM == "" ) { KVIEWPGM = ZVIEWPGM ; } ;
		if ( KFLSTPGM == "" ) { KFLSTPGM = ZFLSTPGM ; } ;
		if ( KHELPPGM == "" ) { KHELPPGM = ZHELPPGM ; } ;
		if ( KOREXPGM == "" ) { KOREXPGM = ZOREXPGM ; } ;

		if ( ZCMD == "CANCEL" ) { break ; }
		if ( ZCMD == "DEFAULTS" )
		{
			KMAINPGM = ZMAINPGM ;
			KPANLPGM = ZPANLPGM ;
			KEDITPGM = ZEDITPGM ;
			KBRPGM   = ZBRPGM   ;
			KVIEWPGM = ZVIEWPGM ;
			KFLSTPGM = ZFLSTPGM ;
			KHELPPGM = ZHELPPGM ;
			KOREXPGM = ZOREXPGM ;
		}

		if ( RCode == 8 || ZCMD == "SAVE" )
		{
			vput( v_list, PROFILE ) ;
			if ( RCode == 8 ) { break ; }
		}
	}
	vdelete( v_list ) ;
}

void PPSP01A::keylistTables()
{
	// Show a list of all key list tables in the ZUPROF path
	// If there are no tables found, create an empty ISPKEYP

	string MSG  ;
	string cw   ;
	string w2   ;
	string tab  ;
	string fname;
	string p    ;

	string TBK1SEL ;
	string TBK1TAB ;
	string TBK1TYP ;
	string TBK1MSG ;
	string KEYP    ;
	string UPROF   ;
	string NEWTAB  ;

	string AKTAB   ;
	string AKLIST  ;

	typedef vector<path> vec ;
	vec v ;

	vec::const_iterator it ;

	vdefine( "TBK1SEL TBK1TAB TBK1TYP TBK1MSG NEWTAB", &TBK1SEL, &TBK1TAB, &TBK1TYP, &TBK1MSG, &NEWTAB ) ;
	KEYP = "KEYP" + right( d2ds( taskid() ), 4, '0' ) ;

	tbcreate( KEYP, "", "TBK1SEL TBK1TAB TBK1TYP TBK1MSG", NOWRITE ) ;
	if ( RC > 0 ) { abend() ; }

	tbsort( KEYP, "TBK1TAB,C,A" ) ;

	vcopy( "ZUPROF", UPROF, MOVE ) ;
	vcopy( "ZKLNAME", AKLIST, MOVE ) ;
	vcopy( "ZKLAPPL", AKTAB, MOVE ) ;

	copy( directory_iterator( UPROF ), directory_iterator(), back_inserter( v ) ) ;

	for ( it = v.begin() ; it != v.end() ; ++it )
	{
		fname = (*it).string() ;
		p     = substr( fname, 1, (lastpos( "/", fname ) - 1) ) ;
		tab   = substr( fname, (lastpos( "/", fname ) + 1) )    ;
		if ( tab.size() < 5 ) { continue ; }
		if ( tab.compare( tab.size()-4, 4, "KEYP" ) == 0 )
		{
			tbvclear( KEYP ) ;
			TBK1TAB = tab    ;
			if ( TBK1TAB == AKTAB+"KEYP" )
			{
				TBK1MSG = "*Active*" ;
			}
			TBK1TYP = "Private" ;
			tbadd( KEYP, "", "ORDER" ) ;
		}
	}

	tbtop( KEYP )     ;
	tbskip( KEYP, 1 ) ;
	if ( RC == 8 )
	{
		NEWTAB = "ISP"   ;
		createKeyTable( NEWTAB ) ;
		tbvclear( KEYP ) ;
		TBK1TAB = NEWTAB+"KEYP" ;
		tbadd( KEYP )    ;
	}

	MSG    = "" ;
	ZTDTOP = 1  ;
	while ( true )
	{
		tbtop( KEYP )     ;
		tbskip( KEYP, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD = "" ; }
		tbdispl( KEYP, "PPSP01K1", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		while ( ZTDSELS > 0 )
		{
			if ( TBK1SEL == "D" )
			{
				display( "PPSP01K7", MSG, "ZCMD" ) ;
				if ( RC == 0 )
				{
					remove( UPROF + "/" + TBK1TAB ) ;
					tbdelete( KEYP ) ;
					if ( RC > 0 ) { abend() ; }
				}
			}
			else if ( TBK1SEL == "N" )
			{
				display( "PPSP01K5", MSG, "ZCMD" ) ;
				if ( RC == 0 )
				{
					TBK1TAB = NEWTAB + "KEYP" ;
					TBK1MSG = "*Added*" ;
					tbadd( KEYP, "", "ORDER" ) ;
					createKeyTable( NEWTAB ) ;
					TBK1SEL = ""  ;
					tbput( KEYP ) ;
				}
			}
			else if ( TBK1SEL == "S" )
			{
				control( "DISPLAY", "SAVE" ) ;
				keylistTable( TBK1TAB, AKTAB, AKLIST ) ;
				control( "DISPLAY", "RESTORE" ) ;
				TBK1MSG = "*Selected*" ;
				TBK1SEL = ""  ;
				tbput( KEYP ) ;
			}
			if ( ZTDSELS > 1 )
			{
				tbdispl( KEYP ) ;
				if ( RC > 4 ) { break ; }
			}
			else { ZTDSELS = 0 ; }
		}
	}
	vdelete( "TBK1SEL TBK1TAB TBK1TYP TBK1MSG NEWTAB" ) ;
	return ;
}


void PPSP01A::keylistTable( string tab, string AKTAB, string AKLIST )
{
	// Show keylist table (default current profile)
	// If there are no rows in table tab, create an empty ISPDEF entry

	int i ;

	string MSG  ;
	string cw   ;
	string w2   ;
	string fname;
	string p    ;
	string t    ;

	string TBK2SEL  ;
	string TBK2LST  ;
	string TBK2MSG  ;
	string KEYLISTN ;
	string KLST     ;
	string UPROF    ;
	string NEWKEY   ;

	bool   actTab   ;

	if ( AKTAB == "" )
	{
		vcopy( "ZKLNAME", AKLIST, MOVE ) ;
		vcopy( "ZKLAPPL", AKTAB, MOVE ) ;
		actTab = true   ;
		if ( AKTAB == "" )
		{
			vcopy( "ZAPPLID", AKTAB, MOVE ) ;
			actTab = false ;
		}
	}
	else
	{
		actTab = ( AKTAB+"KEYP" == tab ) ;
	}

	if ( tab == "" )
	{
		tab = AKTAB+"KEYP" ;
		vreplace( "TBK1TAB", tab ) ;
	}

	vdefine( "TBK2SEL TBK2LST TBK2MSG KEYLISTN NEWKEY", &TBK2SEL, &TBK2LST, &TBK2MSG, &KEYLISTN, &NEWKEY ) ;
	KLST = "KLT2" + right( d2ds( taskid() ), 4, '0' ) ;
	vcopy( "ZUPROF", UPROF, MOVE ) ;

	tbopen( tab, NOWRITE, UPROF ) ;
	if ( RC > 0 ) { abend() ; }

	tbcreate( KLST, "", "TBK2SEL TBK2LST TBK2MSG", NOWRITE ) ;
	if ( RC > 0 ) { abend() ; }

	tbtop( tab ) ;
	tbskip( tab, 1 ) ;
	if ( RC > 0 )
	{
		tbend( tab ) ;
		tbopen( tab, WRITE, UPROF ) ;
		if ( RC > 0 ) { abend() ; }
		tbsort( tab, "KEYLISTN,C,A" ) ;
		KEYLISTN = "ISPDEF" ;
		for ( i = 1 ; i < 25 ; i++ )
		{
			vreplace( "KEY"+d2ds(i)+"DEF", "" ) ;
			vreplace( "KEY"+d2ds(i)+"ATR", "" ) ;
			vreplace( "KEY"+d2ds(i)+"LAB", "" ) ;
		}
		vreplace( "KEYHELPN", "" ) ;
		tbadd( tab, "", "ORDER" ) ;
		if ( RC > 0 ) { abend() ; }
		tbclose( tab ) ;
		tbopen( tab, NOWRITE, UPROF ) ;
		if ( RC > 0 ) { abend() ; }
	}

	tbtop( tab ) ;
	while ( true )
	{
		tbskip( tab ) ;
		if ( RC > 0 ) { break ; }
		tbvclear( KLST ) ;
		TBK2LST = KEYLISTN ;
		if ( actTab && TBK2LST == AKLIST )
		{
			TBK2MSG = "*Active*" ;
		}
		tbadd( KLST ) ;
		if ( RC > 0 ) { abend() ; }
	}
	tbend( tab ) ;

	tbsort( KLST, "TBK2LST,C,A" ) ;

	ZTDTOP = 1 ;
	while ( true )
	{
		tbtop( KLST ) ;
		tbskip( KLST, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD = "" ; }
		tbdispl( KLST, "PPSP01K2", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		while ( ZTDSELS > 0 )
		{
			if ( TBK2SEL == "D" )
			{
				display( "PPSP01K7", MSG, "ZCMD" ) ;
				if ( RC == 0 )
				{
					tbopen( tab, WRITE, UPROF ) ;
					if ( RC > 0 ) { abend() ; }
					KEYLISTN = TBK2LST ;
					tbdelete( tab )   ;
					if ( RC > 0 ) { abend() ; }
					tbdelete( KLST ) ;
					if ( RC > 0 ) { abend() ; }
					tbclose( tab ) ;
				}
			}
			else if ( TBK2SEL == "N" )
			{
				display( "PPSP01K4", MSG, "ZCMD" ) ;
				if ( RC == 0 )
				{
					tbopen( tab, NOWRITE, UPROF ) ;
					if ( RC > 0 ) { abend() ; }
					tbvclear( tab ) ;
					KEYLISTN = NEWKEY ;
					tbget( tab ) ;
					if ( RC == 0 )
					{
						tbend( tab ) ;
						TBK2MSG = "*Exists*" ;
					}
					else
					{
						tbend( tab ) ;
						tbopen( tab, WRITE, UPROF ) ;
						if ( RC > 0 ) { abend() ; }
						tbsort( tab, "KEYLISTN,C,A" ) ;
						KEYLISTN = NEWKEY ;
						for ( i = 1 ; i < 25 ; i++ )
						{
							vreplace( "KEY"+d2ds(i)+"DEF", "" ) ;
							vreplace( "KEY"+d2ds(i)+"ATR", "" ) ;
							vreplace( "KEY"+d2ds(i)+"LAB", "" ) ;
						}
						vcopy( "KEYHELP", t, MOVE ) ;
						vreplace( "KEYHELPN", t ) ;
						tbadd( tab, "", "ORDER" ) ;
						if ( RC > 0 ) { abend() ; }
						tbclose( tab ) ;
						TBK2LST = NEWKEY ;
						TBK2MSG = "*Added*" ;
						tbadd( KLST, "", "ORDER" ) ;
						if ( RC > 0 ) { abend() ; }
						editKeylist( tab, NEWKEY ) ;
					}
					TBK2SEL = ""  ;
					tbput( KLST ) ;
				}
			}
			else if ( TBK2SEL == "E" )
			{
				editKeylist( tab, TBK2LST ) ;
				TBK2MSG = "*Edited*" ;
				TBK2SEL = ""  ;
				tbput( KLST ) ;
			}
			else if ( TBK2SEL == "V" )
			{
				viewKeylist( tab, TBK2LST ) ;
				TBK2MSG = "*Viewed*" ;
				TBK2SEL = ""  ;
				tbput( KLST ) ;
			}
			if ( ZTDSELS > 1 )
			{
				tbdispl( KLST ) ;
				if ( RC > 4 ) { break ; }
			}
			else { ZTDSELS = 0 ; }
		}
	}
	tbend( KLST ) ;
	vdelete( "TBK2SEL TBK2LST TBK2MSG KEYLISTN NEWKEY" ) ;
}


void PPSP01A::viewKeylist( const string& tab, const string& list )
{
	// Field names: KEYLISTN KEYnDEF KEYnLAB KEYnATR (n=1 to 24)
	// TD Field names: KEYNUM KEYDEF KEYATTR KEYLAB

	// Read keylist from table tab, KEYLISTN list and create table display.

	int i ;

	string t        ;
	string KEYNUM   ;
	string KEYDEF   ;
	string KEYATTR  ;
	string KEYLAB   ;

	string KEYLISTN ;
	string KLST     ;
	string UPROF    ;
	string MSG      ;

	KLST = "KLT4" + right( d2ds( taskid() ), 4, '0' ) ;

	vcopy( "ZUPROF", UPROF, MOVE ) ;

	tbopen( tab, NOWRITE, UPROF ) ;
	if ( RC > 0 ) { abend() ; }

	vdefine( "KEYLISTN KEYNUM KEYDEF KEYATTR KEYLAB", &KEYLISTN, &KEYNUM, &KEYDEF, &KEYATTR, &KEYLAB ) ;

	tbcreate( KLST, "KEYNUM", "KEYDEF KEYATTR KEYLAB", NOWRITE ) ;
	if ( RC > 0 ) { abend() ; }

	tbvclear( tab ) ;
	KEYLISTN = list ;
	tbget( tab ) ;
	if ( RC > 0 ) { abend() ; }

	vcopy( "KEYHELPN", t, MOVE ) ;
	vreplace( "KEYHELP", t ) ;
	for ( i = 1 ; i < 25 ; i++ )
	{
		KEYNUM = "F"+left( d2ds( i ), 2 ) + ". . ." ;
		vcopy( "KEY"+d2ds(i)+"DEF", KEYDEF, MOVE ) ;
		vcopy( "KEY"+d2ds(i)+"ATR", KEYATTR, MOVE ) ;
		vcopy( "KEY"+d2ds(i)+"LAB", KEYLAB, MOVE ) ;
		tbadd( KLST ) ;
		if ( RC > 0 ) { abend() ; }
	}
	tbend( tab ) ;

	ZTDTOP = 1 ;
	while ( true )
	{
		tbtop( KLST ) ;
		tbskip( KLST, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD = "" ; }
		tbdispl( KLST, "PPSP01K6", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
	}
	tbend( KLST )  ;
	vdelete( "KEYLISTN KEYNUM KEYDEF KEYATTR KEYLAB" ) ;
}


void PPSP01A::editKeylist( const string& tab, const string& list )
{
	// Field names: KEYLISTN KEYnDEF KEYnLAB KEYnATR (n=1 to 24)
	// TD Field names: KEYNUM KEYDEF KEYATTR KEYLAB

	// Read keylist from table tab, KEYLISTN list and create table display.
	// Update tab/list from table display.

	int i ;

	string t        ;
	string KEYNUM   ;
	string KEYDEF   ;
	string KEYATTR  ;
	string KEYLAB   ;

	string KEYLISTN ;
	string KLST     ;
	string UPROF    ;
	string MSG      ;

	KLST = "KLT3" + right( d2ds( taskid() ), 4, '0' ) ;

	vcopy( "ZUPROF", UPROF, MOVE ) ;

	tbopen( tab, NOWRITE, UPROF ) ;
	if ( RC > 0 ) { abend() ; }

	vdefine( "KEYLISTN KEYNUM KEYDEF KEYATTR KEYLAB", &KEYLISTN, &KEYNUM, &KEYDEF, &KEYATTR, &KEYLAB ) ;

	tbcreate( KLST, "KEYNUM", "KEYDEF KEYATTR KEYLAB", NOWRITE ) ;
	if ( RC > 0 ) { abend() ; }

	tbvclear( tab ) ;
	KEYLISTN = list ;
	tbget( tab ) ;
	if ( RC > 0 ) { abend() ; }

	vcopy( "KEYHELPN", t, MOVE ) ;
	vreplace( "KEYHELP", t ) ;
	for ( i = 1 ; i < 25 ; i++ )
	{
		KEYNUM = "F"+left( d2ds( i ), 2 ) + ". . ." ;
		vcopy( "KEY"+d2ds(i)+"DEF", KEYDEF, MOVE ) ;
		vcopy( "KEY"+d2ds(i)+"ATR", KEYATTR, MOVE ) ;
		vcopy( "KEY"+d2ds(i)+"LAB", KEYLAB, MOVE ) ;
		tbadd( KLST ) ;
		if ( RC > 0 ) { abend() ; }
	}
	tbend( tab ) ;

	ZTDTOP = 1 ;
	while ( true )
	{
		tbtop( KLST ) ;
		tbskip( KLST, ZTDTOP ) ;
		if ( MSG == "" ) { ZCMD = "" ; }
		tbdispl( KLST, "PPSP01K3", MSG, "ZCMD" ) ;
		if ( RC  >  8 ) { abend() ; }
		if ( RC ==  8 ) { break   ; }
		MSG = "" ;
		while ( ZTDSELS > 0 )
		{
			tbmod( KLST ) ;
			if ( ZTDSELS > 1 )
			{
				tbdispl( KLST ) ;
				if ( RC > 4 ) { break ; }
			}
			else { ZTDSELS = 0 ; }
		}
	}

	tbopen( tab, WRITE, UPROF ) ;
	if ( RC > 0 ) { abend() ; }

	tbvclear( tab ) ;
	KEYLISTN = list ;
	tbget( tab ) ;
	if ( RC > 0 ) { abend() ; }

	vcopy( "KEYHELP", t, MOVE ) ;
	vreplace( "KEYHELPN", t ) ;
	tbtop( KLST ) ;
	for ( i = 1 ; i < 25 ; i++ )
	{
		tbskip( KLST, 1 ) ;
		if ( RC > 0 ) { break ; }
		vreplace( "KEY"+d2ds(i)+"DEF", KEYDEF ) ;
		vreplace( "KEY"+d2ds(i)+"ATR", KEYATTR ) ;
		vreplace( "KEY"+d2ds(i)+"LAB", KEYLAB ) ;
	}

	tbmod( tab )   ;
	if ( RC > 0 ) { abend() ; }

	tbclose( tab ) ;
	if ( RC > 0 ) { abend() ; }

	tbend( KLST )  ;

	vdelete( "KEYLISTN KEYNUM KEYDEF KEYATTR KEYLAB" ) ;
}


void PPSP01A::createKeyTable( string tab )
{
	// Create an empty keylist table entry

	int i ;

	string UPROF  ;
	string flds   ;

	vcopy( "ZUPROF", UPROF, MOVE ) ;
	tab += "KEYP" ;
	flds = ""     ;

	for ( i = 1 ; i < 25 ; i++ )
	{
		flds += "KEY"+d2ds(i)+"DEF " ;
		flds += "KEY"+d2ds(i)+"ATR " ;
		flds += "KEY"+d2ds(i)+"LAB " ;
	}
	flds += "KEYHELPN" ;

	tbcreate( tab, "KEYLISTN", flds, WRITE, NOREPLACE, UPROF ) ;
	if ( RC > 0 ) { abend() ; }

	tbsave( tab ) ;
	if ( RC > 0 ) { abend() ; }

	tbend( tab ) ;
}


void PPSP01A::runApplication( const string& appl )
{
	select( "PGM("+appl+") NEWAPPL(ISP) NEWPOOL PASSLIB" ) ;
}


// ============================================================================================ //

extern "C" { pApplication *maker() { return new PPSP01A ; } }
extern "C" { void destroy(pApplication *p) { delete p ; } }
