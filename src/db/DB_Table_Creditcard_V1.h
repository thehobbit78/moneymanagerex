// -*- C++ -*-
//=============================================================================
/**
 *      Copyright (c) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 *
 *      @file
 *
 *      @author [sqlite2cpp.py]
 *
 *      @brief
 *
 *      Revision History:
 *          AUTO GENERATED at 2014-11-12 14:20:38.465922.
 *          DO NOT EDIT!
 */
//=============================================================================

#ifndef DB_TABLE_CREDITCARD_V1_H
#define DB_TABLE_CREDITCARD_V1_H

#include "DB_Table.h"

struct DB_Table_CREDITCARD_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_CREDITCARD_V1 Self;
    /** A container to hold list of Data records for the table*/
    struct Data_Set : public std::vector<Self::Data>
    {
        std::wstring to_json() const
        {
            json::Array a;
            for (const auto & item: *this)
            {
                json::Object o;
                item.to_json(o);
                a.Insert(o);
            }
            std::wstringstream ss;
            json::Writer::Write(a, ss);
            return ss.str();
        }
    };
    /** A container to hold a list of Data record pointers for the table in memory*/
    typedef std::vector<Self::Data*> Cache;
    typedef std::map<int, Self::Data*> Index_By_Id;
    Cache cache_;
    Index_By_Id index_by_id_;

    /** Destructor: clears any data records stored in memory */
    ~DB_Table_CREDITCARD_V1() 
    {
        destroy_cache();
    }
	 
    /** Removes all records stored in memory (cache) for the table*/ 
    void destroy_cache()
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fun(&Data::destroy));
        cache_.clear();
        index_by_id_.clear(); // no memory release since it just stores pointer and the according objects are in cache
    }

    /** Creates the database table if the table does not exist*/
    bool ensure(wxSQLite3Database* db)
    {
        if (!exists(db))
		{
			try
			{
				db->ExecuteUpdate("CREATE TABLE CREDITCARD_V1(CARDID integer primary key, ACCOUNTID integer NOT NULL UNIQUE, HELDAT integer, CARDTYPE integer, RATE numeric, CARDLIMIT numeric)");
			}
			catch(const wxSQLite3Exception &e) 
			{ 
				wxLogError("CREDITCARD_V1: Exception %s", e.GetMessage().c_str());
				return false;
			}
		}

        this->ensure_index(db);

        return true;
    }

    bool ensure_index(wxSQLite3Database* db)
    {
        try
        {
            db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS IDX_CREDITCARD_ACCOUNTID ON CREDITCARD_V1 (ACCOUNTID)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("CREDITCARD_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    struct CARDID : public DB_Column<int>
    { 
        static wxString name() { return "CARDID"; } 
        explicit CARDID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct ACCOUNTID : public DB_Column<int>
    { 
        static wxString name() { return "ACCOUNTID"; } 
        explicit ACCOUNTID(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct HELDAT : public DB_Column<int>
    { 
        static wxString name() { return "HELDAT"; } 
        explicit HELDAT(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct CARDTYPE : public DB_Column<int>
    { 
        static wxString name() { return "CARDTYPE"; } 
        explicit CARDTYPE(const int &v, OP op = EQUAL): DB_Column<int>(v, op) {}
    };
    struct RATE : public DB_Column<double>
    { 
        static wxString name() { return "RATE"; } 
        explicit RATE(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    struct CARDLIMIT : public DB_Column<double>
    { 
        static wxString name() { return "CARDLIMIT"; } 
        explicit CARDLIMIT(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    typedef CARDID PRIMARY;
    enum COLUMN
    {
        COL_CARDID = 0
        , COL_ACCOUNTID = 1
        , COL_HELDAT = 2
        , COL_CARDTYPE = 3
        , COL_RATE = 4
        , COL_CARDLIMIT = 5
    };

    /** Returns the column name as a string*/
    static wxString column_to_name(COLUMN col)
    {
        switch(col)
        {
            case COL_CARDID: return "CARDID";
            case COL_ACCOUNTID: return "ACCOUNTID";
            case COL_HELDAT: return "HELDAT";
            case COL_CARDTYPE: return "CARDTYPE";
            case COL_RATE: return "RATE";
            case COL_CARDLIMIT: return "CARDLIMIT";
            default: break;
        }
        
        return "UNKNOWN";
    }

    /** Returns the comumn number from the given column name*/
    static COLUMN name_to_column(const wxString& name)
    {
        if ("CARDID" == name) return COL_CARDID;
        else if ("ACCOUNTID" == name) return COL_ACCOUNTID;
        else if ("HELDAT" == name) return COL_HELDAT;
        else if ("CARDTYPE" == name) return COL_CARDTYPE;
        else if ("RATE" == name) return COL_RATE;
        else if ("CARDLIMIT" == name) return COL_CARDLIMIT;

        return COLUMN(-1);
    }
    
    /** Data is a single record in the database table*/
    struct Data
    {
        friend struct DB_Table_CREDITCARD_V1;
        /** This is a instance pointer to itself in memory. */
        Self* view_;
    
        int CARDID;//  primay key
        int ACCOUNTID;
        int HELDAT;
        int CARDTYPE;
        double RATE;
        double CARDLIMIT;
        int id() const { return CARDID; }
        void id(int id) { CARDID = id; }
        bool operator < (const Data& r) const
        {
            return this->id() < r.id();
        }
        bool operator < (const Data* r) const
        {
            return this->id() < r->id();
        }

        explicit Data(Self* view = 0) 
        {
            view_ = view;
        
            CARDID = -1;
            ACCOUNTID = -1;
            HELDAT = -1;
            CARDTYPE = -1;
            RATE = 0.0;
            CARDLIMIT = 0.0;
        }

        explicit Data(wxSQLite3ResultSet& q, Self* view = 0)
        {
            view_ = view;
        
            CARDID = q.GetInt(0); // CARDID
            ACCOUNTID = q.GetInt(1); // ACCOUNTID
            HELDAT = q.GetInt(2); // HELDAT
            CARDTYPE = q.GetInt(3); // CARDTYPE
            RATE = q.GetDouble(4); // RATE
            CARDLIMIT = q.GetDouble(5); // CARDLIMIT
        }

        Data& operator=(const Data& other)
        {
            if (this == &other) return *this;

            CARDID = other.CARDID;
            ACCOUNTID = other.ACCOUNTID;
            HELDAT = other.HELDAT;
            CARDTYPE = other.CARDTYPE;
            RATE = other.RATE;
            CARDLIMIT = other.CARDLIMIT;
            return *this;
        }

        template<typename C>
        bool match(const C &c) const
        {
            return false;
        }
        bool match(const Self::CARDID &in) const
        {
            return this->CARDID == in.v_;
        }
        bool match(const Self::ACCOUNTID &in) const
        {
            return this->ACCOUNTID == in.v_;
        }
        bool match(const Self::HELDAT &in) const
        {
            return this->HELDAT == in.v_;
        }
        bool match(const Self::CARDTYPE &in) const
        {
            return this->CARDTYPE == in.v_;
        }
        bool match(const Self::RATE &in) const
        {
            return this->RATE == in.v_;
        }
        bool match(const Self::CARDLIMIT &in) const
        {
            return this->CARDLIMIT == in.v_;
        }
        wxString to_json() const
        {
            json::Object o;
            this->to_json(o);
            std::wstringstream ss;
            json::Writer::Write(o, ss);
            return ss.str();
        }
        
        int to_json(json::Object& o) const
        {
            o[L"CARDID"] = json::Number(this->CARDID);
            o[L"ACCOUNTID"] = json::Number(this->ACCOUNTID);
            o[L"HELDAT"] = json::Number(this->HELDAT);
            o[L"CARDTYPE"] = json::Number(this->CARDTYPE);
            o[L"RATE"] = json::Number(this->RATE);
            o[L"CARDLIMIT"] = json::Number(this->CARDLIMIT);
            return 0;
        }
        row_t to_row_t() const
        {
            row_t row;
            row(L"CARDID") = CARDID;
            row(L"ACCOUNTID") = ACCOUNTID;
            row(L"HELDAT") = HELDAT;
            row(L"CARDTYPE") = CARDTYPE;
            row(L"RATE") = RATE;
            row(L"CARDLIMIT") = CARDLIMIT;
            return row;
        }
        void to_template(html_template& t) const
        {
            t(L"CARDID") = CARDID;
            t(L"ACCOUNTID") = ACCOUNTID;
            t(L"HELDAT") = HELDAT;
            t(L"CARDTYPE") = CARDTYPE;
            t(L"RATE") = RATE;
            t(L"CARDLIMIT") = CARDLIMIT;
        }

        /** Save the record instance in memory to the database. */
        bool save(wxSQLite3Database* db)
        {
            if (db && db->IsReadOnly()) return false;
            if (!view_ || !db) 
            {
                wxLogError("can not save CREDITCARD_V1");
                return false;
            }

            return view_->save(this, db);
        }

        /** Remove the record instance from memory and the database. */
        bool remove(wxSQLite3Database* db)
        {
            if (!view_ || !db) 
            {
                wxLogError("can not remove CREDITCARD_V1");
                return false;
            }
            
            return view_->remove(this, db);
        }

        void destroy()
        {
            //if (this->id() < 0)
            //    wxSafeShowMessage("unsaved object", this->to_json());
            delete this;
        }
    };

    enum
    {
        NUM_COLUMNS = 8
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    /** Name of the table*/    
    wxString name() const { return "CREDITCARD_V1"; }

    DB_Table_CREDITCARD_V1() 
    {
        query_ = "SELECT * FROM CREDITCARD_V1 ";
    }

    /** Create a new Data record and add to memory table (cache)*/
    Self::Data* create()
    {
        Self::Data* entity = new Self::Data(this);
        cache_.push_back(entity);
        return entity;
    }
    
    /** Create a copy of the Data record and add to memory table (cache)*/
    Self::Data* clone(const Data* e)
    {
        Self::Data* entity = create();
        *entity = *e;
        entity->id(-1);
        return entity;
    }

    /**
    * Saves the Data record to the database table.
    * Either create a new record or update the existing record.
    * Remove old record from the memory table (cache)
    */
    bool save(Self::Data* entity, wxSQLite3Database* db)
    {
        wxString sql = wxEmptyString;
        if (entity->id() <= 0) //  new & insert
        {
            sql = "INSERT INTO CREDITCARD_V1(ACCOUNTID, HELDAT, CARDTYPE, RATE, CARDLIMIT) VALUES(?, ?, ?, ?, ?)";
        }
        else
        {
            sql = "UPDATE CREDITCARD_V1 SET ACCOUNTID = ?, HELDAT = ?, CARDTYPE = ?, RATE = ?, CARDLIMIT = ? WHERE CARDID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->ACCOUNTID);
            stmt.Bind(2, entity->HELDAT);
            stmt.Bind(3, entity->CARDTYPE);
            stmt.Bind(4, entity->RATE);
            stmt.Bind(5, entity->CARDLIMIT);
            if (entity->id() > 0)
                stmt.Bind(6, entity->CARDID);

            stmt.ExecuteUpdate();
            stmt.Finalize();

            if (entity->id() > 0) // existent
            {
                for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
                {
                    Self::Data* e = *it;
                    if (e->id() == entity->id()) 
                        *e = *entity;  // in-place update
                }
            }
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("CREDITCARD_V1: Exception %s, %s", e.GetMessage().c_str(), entity->to_json());
            return false;
        }

        if (entity->id() <= 0)
        {
            entity->id((db->GetLastRowId()).ToLong());
            index_by_id_.insert(std::make_pair(entity->id(), entity));
        }
        return true;
    }

    /** Remove the Data record from the database and the memory table (cache) */
    bool remove(int id, wxSQLite3Database* db)
    {
        if (id <= 0) return false;
        try
        {
            wxString sql = "DELETE FROM CREDITCARD_V1 WHERE CARDID = ?";
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();

            Cache c;
            for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
            {
                Self::Data* entity = *it;
                if (entity->id() == id) 
                {
                    index_by_id_.erase(entity->id());
                    delete entity;
                }
                else 
                {
                    c.push_back(entity);
                }
            }
            cache_.clear();
            cache_.swap(c);
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("CREDITCARD_V1: Exception %s", e.GetMessage().c_str());
            return false;
        }

        return true;
    }

    /** Remove the Data record from the database and the memory table (cache) */
    bool remove(Self::Data* entity, wxSQLite3Database* db)
    {
        if (remove(entity->id(), db))
        {
            entity->id(-1);
            return true;
        }

        return false;
    }

    template<typename... Args>
    Self::Data* get_one(const Args& ... args)
    {
        for (Index_By_Id::iterator it = index_by_id_.begin(); it != index_by_id_.end(); ++ it)
        {
            Self::Data* item = it->second;
            if (item->id() > 0 && match(item, args...)) 
            {
                ++ hit_;
                return item;
            }
        }

        ++ miss_;

        return 0;
    }
    
    /**
    * Search the memory table (Cache) for the data record.
    * If not found in memory, search the database and update the cache.
    */
    Self::Data* get(int id, wxSQLite3Database* db)
    {
        if (id <= 0) 
        {
            ++ skip_;
            return 0;
        }

        Index_By_Id::iterator it = index_by_id_.find(id);
        if (it != index_by_id_.end())
        {
            ++ hit_;
            return it->second;
        }
        
        ++ miss_;
        Self::Data* entity = 0;
        wxString where = wxString::Format(" WHERE %s = ?", PRIMARY::name().c_str());
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
            stmt.Bind(1, id);

            wxSQLite3ResultSet q = stmt.ExecuteQuery();
            if(q.NextRow())
            {
                entity = new Self::Data(q, this);
                cache_.push_back(entity);
                index_by_id_.insert(std::make_pair(id, entity));
            }
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().c_str(), e.GetMessage().c_str());
        }
        
        if (!entity) 
        {
            wxLogError("%s: %d not found", this->name().c_str(), id);
        }
 
        return entity;
    }

    /**
    * Return a list of Data records (Data_Set) derived directly from the database.
    * The Data_Set is sorted based on the column number.
    */
    const Data_Set all(wxSQLite3Database* db, COLUMN col = COLUMN(0), bool asc = true)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(col == COLUMN(0) ? this->query() : this->query() + " ORDER BY " + column_to_name(col) + " COLLATE NOCASE " + (asc ? " ASC " : " DESC "));

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(entity);
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().c_str(), e.GetMessage().c_str());
        }

        return result;
    }
};
#endif //
