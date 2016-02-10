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

#ifndef MODEL_CREDITCARD_H
#define MODEL_CREDITCARD_H

#include "Model.h"
#include "db/DB_Table_Creditcard_V1.h"

class Model_CreditCard : public Model<DB_Table_CREDITCARD_V1>
{
public:
    using Model<DB_Table_CREDITCARD_V1>::get;
    enum TYPE { NOCHARGE = 0, CHARGEPARTLY, CHARGEALL };

    static const std::vector<std::pair<TYPE, wxString>> TYPE_CHOICES;

public:
    Model_CreditCard();
    ~Model_CreditCard();

public:
    /**
    Initialize the global Model_CreditCard table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for Model_CreditCard table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_CreditCard& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for Model_CreditCard table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_CreditCard& instance();

public:
    static wxArrayString all_type();

    Data* get(int accountId);
    /**
    * Return the credit card automated repeating transaction
    */
    static int getRepeatingTransaction(int cardAcctId, int heldAtAcct=-1);

    /**
    * Return the credit card monthly balance at a certain date
    */
    static double getCardBalanceAt(int cardAcctId, wxDate month);
};

#endif // 
