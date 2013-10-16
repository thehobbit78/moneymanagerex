/*******************************************************
 Copyright (C) 2013 Nikolay

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

#include "html_widget_top_categories.h"

#include "htmlbuilder.h"
#include "util.h"
#include "model/Model_Checking.h"
#include "model/Model_Category.h"
#include "model/Model_Subcategory.h"
#include "model/Model_Splittransaction.h"

#include <algorithm>

htmlWidgetTop7Categories::htmlWidgetTop7Categories(mmDateRange* date_range)
    : date_range_(date_range)
{
    title_ = wxString::Format(_("Top Withdrawals: %s"), date_range_->title());
}

htmlWidgetTop7Categories::~htmlWidgetTop7Categories()
{
    if (date_range_) delete date_range_;
}

wxString htmlWidgetTop7Categories::getHTMLText()
{
    mmHTMLBuilder hb;

    hb.startTable("100%");
    hb.addTableHeaderRow(title_, 2);
    hb.startTableRow();
    hb.addTableCell(_("Category"), false, false, true);
    //hb.addTableCell(_("QTY"), true, false, true);
    hb.addTableCell(_("Summary"), true, false, true);
    hb.endTableRow();

    std::vector<std::pair<wxString, double> > topCategoryStats;
    getTopCategoryStats(topCategoryStats, date_range_);

    for (const auto& i : topCategoryStats)
    {
        hb.startTableRow();
        hb.addTableCell((i.first.IsEmpty() ? "..." : i.first), false, true);
        hb.addMoneyCell(i.second);
        hb.endTableRow();
    }

    return hb.getHTMLinTableWraper(true);
}

void htmlWidgetTop7Categories::getTopCategoryStats(
    std::vector<std::pair<wxString, double> > &categoryStats
    , mmDateRange* date_range) const
{
    //Get base currency rates for all accounts
    std::map<int, double> acc_conv_rates;
    for (const auto& account: Model_Account::instance().all())
    {
        Model_Currency::Data* currency = Model_Account::currency(account);
        acc_conv_rates[account.ACCOUNTID] = currency->BASECONVRATE;
    }
    //Temporary map
    std::map<wxString, double> stat;

    Model_Checking::Data_Set transactions = Model_Checking::instance().find(
            DB_Table_CHECKINGACCOUNT_V1::TRANSDATE(date_range->start_date().FormatISODate()
                , DB_Table_CHECKINGACCOUNT_V1::TRANSDATE::GREATER_OR_EQUAL)
            , DB_Table_CHECKINGACCOUNT_V1::TRANSDATE(date_range->end_date().FormatISODate()
                , DB_Table_CHECKINGACCOUNT_V1::TRANSDATE::LESS_OR_EQUAL));

    wxLogDebug("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    for (const auto &trx : transactions)
    {
        if (Model_Checking::status(trx) ==  Model_Checking::VOID_) continue; // skip
        if (Model_Checking::type(trx) == Model_Checking::TRANSFER) continue; // skip

        if (trx.CATEGID > -1)
        {
            Model_Category::Data* category = Model_Category::instance().get(trx.CATEGID);
            Model_Subcategory::Data* sub_category = Model_Subcategory::instance().get(trx.SUBCATEGID);

            const wxString categ_name = Model_Category::full_name(category, sub_category);
            if (Model_Checking::type(trx) == Model_Checking::DEPOSIT)
                stat[categ_name] += trx.TRANSAMOUNT * (acc_conv_rates[trx.ACCOUNTID]);
            else
                stat[categ_name] -= trx.TRANSAMOUNT * (acc_conv_rates[trx.ACCOUNTID]);
        }
        else
        {
            Model_Splittransaction::Data_Set splits = Model_Splittransaction::instance().find(Model_Splittransaction::TRANSID(trx.TRANSID));
            for (const auto& entry : splits)
            {
                Model_Category::Data* category = Model_Category::instance().get(entry.CATEGID);
                Model_Subcategory::Data* sub_category = (entry.SUBCATEGID != -1 ? Model_Subcategory::instance().get(entry.SUBCATEGID) : 0);

                const wxString categ_name = Model_Category::full_name(category, sub_category);
                stat[categ_name] += entry.SPLITTRANSAMOUNT
                    * (acc_conv_rates[trx.ACCOUNTID]) 
                    * (trx.TRANSAMOUNT< 0 ? -1 : 1);
            }
        }
    }

    categoryStats.clear();
    for (const auto& i : stat)
    {
        if (i.second < 0)
        {
            std::pair <wxString, double> stat_pair;
            stat_pair.first = i.first;
            stat_pair.second = i.second;
            categoryStats.push_back(stat_pair);
        }
    }

    std::stable_sort(categoryStats.begin(), categoryStats.end()
        , [] (const std::pair<wxString, double> x, const std::pair<wxString, double> y)
        { return x.second < y.second; }
    );

    int counter = 0;
    std::vector<std::pair<wxString, double> >::iterator iter;
    for (iter = categoryStats.begin(); iter != categoryStats.end(); )
    {
        counter++;
        if (counter > 7)
            iter = categoryStats.erase(iter);
        else
            ++iter;
    }
}

