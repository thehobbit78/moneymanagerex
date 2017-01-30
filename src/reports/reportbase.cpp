/*******************************************************
 Copyright (C) 2013,2017 James Higley

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

#include "reportbase.h"
#include "constants.h"
#include "mmex.h"
#include "mmDateRange.h"
#include "model/Model_Account.h"
#include "mmSimpleDialogs.h"

mmPrintableBase::mmPrintableBase(const wxString& title)
    : m_title(title)
    , m_date_range(nullptr)
    , m_initial(true)
    , m_date_selection(0)
    , m_account_selection(0)
    , accountArray_(nullptr)
    , m_only_active(false)
{
}

mmPrintableBase::~mmPrintableBase()
{
    if (accountArray_)
        delete accountArray_;
}

void mmPrintableBase::accounts(int selection, wxString& name)
{
    if ((selection == 1) || (m_account_selection != selection))
    {
        m_account_selection = selection;
        if (accountArray_)
        {
            delete accountArray_;
            accountArray_ = nullptr;
        }

        switch (selection)
        {
        case 0: // All Accounts
            break;
        case 1: // Select Accounts
            {
                wxArrayString* accountSelections = new wxArrayString();
                const Model_Account::Data_Set accounts = 
                    (m_only_active ? Model_Account::instance().find(Model_Account::ACCOUNTTYPE(Model_Account::all_type()[Model_Account::INVESTMENT], NOT_EQUAL), Model_Account::STATUS(Model_Account::OPEN))
                    : Model_Account::instance().find(Model_Account::ACCOUNTTYPE(Model_Account::all_type()[Model_Account::INVESTMENT], NOT_EQUAL)));

                mmMultiChoiceDialog mcd(0, _("Choose Accounts"), m_title, accounts);

                if (mcd.ShowModal() == wxID_OK)
                {
                    for (const auto &i : mcd.GetSelections())
                        accountSelections->Add(accounts.at(i).ACCOUNTNAME);
                }

                accountArray_ = accountSelections;
            }
            break;
        default: // All of Account type
            {
                wxArrayString* accountSelections = new wxArrayString();
                auto accounts = Model_Account::instance().find(Model_Account::ACCOUNTTYPE(name));
                for (const auto &i : accounts)
                {
                    if (m_only_active && (i.STATUS == Model_Account::all_status()[Model_Account::CLOSED]))
                        continue;
                    accountSelections->Add(i.ACCOUNTNAME);
                }
                accountArray_ = accountSelections;
            }
        }
    }
}

wxString mmPrintableBase::title() const
{
    if (!m_date_range) 
        return m_title; 
    else 
        return m_title + " - " + m_date_range->title();
}

wxString mmPrintableBase::file_name() const
{
    wxString file_name = title();
    file_name.Replace(" - ", "-");
    file_name.Replace(" ", "_");
    file_name.Replace("/", "-");
    return file_name;
}

mmGeneralReport::mmGeneralReport(const Model_Report::Data* report)
: mmPrintableBase(report->REPORTNAME)
, m_report(report)
{
}

wxString mmGeneralReport::getHTMLText()
{
    return Model_Report::instance().get_html(this->m_report);
}

mm_html_template::mm_html_template(const wxString& arg_template): html_template(arg_template.ToStdWstring())
{
    this->load_context();
}

void mm_html_template::load_context()
{
    (*this)(L"TODAY") = wxDate::Now().FormatISODate();
    for (const auto &r: Model_Infotable::instance().all())
        (*this)(r.INFONAME.ToStdWstring()) = r.INFOVALUE;
    (*this)(L"INFOTABLE") = Model_Infotable::to_loop_t();

    const Model_Currency::Data* currency = Model_Currency::GetBaseCurrency();
    if (currency) currency->to_template(*this);
}
