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

#include "Model_CreditCard.h"
#include "Model_Billsdeposits.h"
#include "Model_Account.h"
#include "wx/translation.h"

const std::vector<std::pair<Model_CreditCard::TYPE, wxString>> Model_CreditCard::TYPE_CHOICES =
{
    std::make_pair(Model_CreditCard::NOCHARGE, wxTRANSLATE("Debit")),
    std::make_pair(Model_CreditCard::CHARGEPARTLY, wxTRANSLATE("Credit (monthly partial refund)")),
    std::make_pair(Model_CreditCard::CHARGEALL, wxTRANSLATE("Credit (monthly full refund)")),
};

Model_CreditCard::Model_CreditCard()
: Model<DB_Table_CREDITCARD_V1>()
{
};

Model_CreditCard::~Model_CreditCard()
{
};

/**
* Initialize the global Model_CreditCard table.
* Reset the Model_CreditCard table or create the table if it does not exist.
*/
Model_CreditCard& Model_CreditCard::instance(wxSQLite3Database* db)
{
    Model_CreditCard& ins = Singleton<Model_CreditCard>::instance();
    ins.db_ = db;
    ins.ensure(db);

    return ins;
}

/** Return the static instance of Model_CreditCard table */
Model_CreditCard& Model_CreditCard::instance()
{
    return Singleton<Model_CreditCard>::instance();
}

wxArrayString Model_CreditCard::all_type()
{
    wxArrayString type;
    for (const auto& item : TYPE_CHOICES)
        type.Add(item.second);
    return type;
}

/** Get the Data record instance in memory. */
Model_CreditCard::Data* Model_CreditCard::get(int accountId)
{
    Data* account = this->get_one(ACCOUNTID(accountId));
    if (account) return account;

    Data_Set items = this->find(ACCOUNTID(accountId));
    if (!items.empty()) account = this->get(items[0].CARDID, this->db_);
    return account;
}

/**
* Return the credit card automated repeating transaction
*/
int Model_CreditCard::getRepeatingTransaction(int cardAcctId, int heldAtAcct/*-1*/)
{
    Model_Billsdeposits::Data_Set dataSet;
    if (heldAtAcct == -1)
        dataSet = Model_Billsdeposits::instance().find(Model_Billsdeposits::TOACCOUNTID(cardAcctId), 
            Model_Billsdeposits::TRANSCODE(Model_Billsdeposits::TRANSFER));
    else
        dataSet = Model_Billsdeposits::instance().find(Model_Billsdeposits::TOACCOUNTID(cardAcctId), 
        Model_Billsdeposits::ACCOUNTID(heldAtAcct), Model_Billsdeposits::TRANSCODE(Model_Billsdeposits::TRANSFER));
    return dataSet.empty() ? 0:dataSet[0].id();
}

/**
* Return the credit card monthly balance at a certain date
*/
double Model_CreditCard::getCardBalanceAt(int cardAcctId, wxDate month)
{
    double balance = 0.0;
    const Model_Account::Data *account = Model_Account::instance().get(cardAcctId);
    const Model_Currency::Data *currency = Model_Account::currency(account);
    wxDate startDate, endDate;
    startDate = endDate = month;
    startDate.SetDay(1);
    endDate.SetToLastMonthDay(endDate.GetMonth(), endDate.GetYear());
    Model_Checking::Data_Set trans = Model_Checking::instance().find(Model_Checking::ACCOUNTID(cardAcctId),
        Model_Checking::TRANSDATE(startDate, GREATER_OR_EQUAL), Model_Checking::TRANSDATE(endDate, LESS_OR_EQUAL));

    for (const auto &t : trans)
        balance += Model_Checking::balance(t, t.ACCOUNTID) * currency->BASECONVRATE;

    return balance;
}
