/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#ifndef MODEL_REPORT_H
#define MODEL_REPORT_H

#include "Model.h"
#include "db/DB_Table_Report_V1.h"

#include "cajun/json/elements.h"
#include "cajun/json/reader.h"
#include "cajun/json/writer.h"

class Model_Report : public Model<DB_Table_REPORT_V1>
{
public:
    using Model<DB_Table_REPORT_V1>::get;

public:
    Model_Report(); 
    ~Model_Report();

public:
    /**
    Initialize the global Model_Report table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for Model_Report table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_Report& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for Model_Report table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_Report& instance();

public:
    bool get_objects_from_sql(const wxString& query, json::Object& o);
    wxArrayString allGroupNames();
    wxString get_html(const Data* r);
    wxString get_html(const Data& r);
    static void prepareTempFolder();
    static bool WindowsUpdateRegistry();
    static void outputReportFile(const wxString& str);

public:
    Data* get(const wxString& name);
};

#endif // 
