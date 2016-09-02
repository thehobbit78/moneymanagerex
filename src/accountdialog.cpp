/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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

#include "accountdialog.h"
#include "attachmentdialog.h"
#include "constants.h"
#include "images_list.h"
#include "maincurrencydialog.h"
#include "mmSimpleDialogs.h"
#include "option.h"
#include "paths.h"
#include "util.h"
#include "validators.h"
#include "webapp.h"

#include "model/Model_Infotable.h"
#include "model/Model_CreditCard.h"
#include "model/Model_Category.h"
#include "model/Model_Currency.h"
#include "model/Model_Attachment.h"

#include <wx/valnum.h>

enum {
    ID_DIALOG_NEWACCT_BUTTON_CURRENCY = wxID_HIGHEST + 1000,
    ID_DIALOG_NEWACCT_TEXTCTRL_ACCTNAME,
    ID_ACCTNUMBER,
    ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT,
    ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE,
    ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT,
    ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO,
    ID_DIALOG_NEWACCT_TEXTCTRL_NOTES,
    ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE,
    ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS,
    ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT,
    ID_DIALOG_NEWACCT_COMBO_ACCTTYPE,
    ID_DIALOG_NEWACCT_COMBO_HELDACCT,
    ID_DIALOG_NEWACCT_COMBO_CARDTYPE,
    ID_DIALOG_NEWACCT_TEXTCTRL_CARDINTERESTRATE,
    ID_DIALOG_NEWACCT_TEXTCTRL_CARDLIMIT,
    ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCE,
    ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEDATE,
    ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEAMNT
};

wxIMPLEMENT_DYNAMIC_CLASS(mmNewAcctDialog, wxDialog);

wxBEGIN_EVENT_TABLE(mmNewAcctDialog, wxDialog)
EVT_BUTTON(wxID_OK, mmNewAcctDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, mmNewAcctDialog::OnCancel)
EVT_BUTTON(ID_DIALOG_NEWACCT_BUTTON_CURRENCY, mmNewAcctDialog::OnCurrency)
EVT_BUTTON(wxID_FILE, mmNewAcctDialog::OnAttachments)
EVT_MENU_RANGE(wxID_HIGHEST, wxID_HIGHEST + acc_img::MAX_XPM, mmNewAcctDialog::OnCustonImage)
EVT_CHOICE(ID_DIALOG_NEWACCT_COMBO_HELDACCT, mmNewAcctDialog::OnCreditCardFields)
EVT_CHOICE(ID_DIALOG_NEWACCT_COMBO_CARDTYPE, mmNewAcctDialog::OnCreditCardFields)
EVT_CHECKBOX(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCE, mmNewAcctDialog::OnCreditCardFields)
EVT_TEXT_ENTER(wxID_ANY, mmNewAcctDialog::OnTextEntered)
wxEND_EVENT_TABLE()

mmNewAcctDialog::mmNewAcctDialog()
{
}

mmNewAcctDialog::mmNewAcctDialog(Model_Account::Data* account, wxWindow* parent, const wxString &name)
    : m_account(account)
    , m_currencyID(-1)
    , m_textAccountName(nullptr)
    , m_notesCtrl(nullptr)
    , m_initbalance_ctrl(nullptr)
    , m_imageList(nullptr)
    , m_bitmapButtons(nullptr)
    , m_statement_lock_ctrl(nullptr)
    , m_statement_date_ctrl(nullptr)
    , m_minimum_balance_ctrl(nullptr)
    , m_credit_limit_ctrl(nullptr)
    , m_interest_rate_ctrl(nullptr)
    , m_payment_due_date_ctrl(nullptr)
    , m_minimum_payment_ctrl(nullptr)
    , m_accessinfo_infocus(false)
{
    m_imageList = navtree_images_list();
    long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX;
    Create(parent, wxID_ANY, _("New Account"), wxDefaultPosition, wxSize(550, 300), style, name);
    this->Connect(wxID_ANY, wxEVT_CHILD_FOCUS, wxChildFocusEventHandler(mmNewAcctDialog::OnChangeFocus), nullptr, this);
}

mmNewAcctDialog::~mmNewAcctDialog()
{
    if (m_imageList)
        delete m_imageList;
}

bool mmNewAcctDialog::Create(wxWindow* parent
    , wxWindowID id
    , const wxString& caption
    , const wxPoint& pos
    , const wxSize& size
    , long style
    , const wxString& name)
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create(parent, id, caption, pos, size, style, name);
    this->SetTitle(_("Edit Account"));
    SetIcon(mmex::getProgramIcon());

    CreateControls();
    fillControls();

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    this->SetInitialSize();
    Centre();

    return TRUE;
}

void mmNewAcctDialog::fillControls()
{
    if (!this->m_account) return;

    m_textAccountName->SetValue(m_account->ACCOUNTNAME);

    wxTextCtrl* textCtrl = (wxTextCtrl*)FindWindow(ID_ACCTNUMBER);
    textCtrl->SetValue(m_account->ACCOUNTNUM);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT);
    textCtrl->SetValue(m_account->HELDAT);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE);
    textCtrl->SetValue(m_account->WEBSITE);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT);
    textCtrl->SetValue(m_account->CONTACTINFO);

    textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_NOTES);
    textCtrl->SetValue(m_account->NOTES);

    wxChoice* itemAcctType = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTTYPE);
    itemAcctType->SetStringSelection(wxGetTranslation(m_account->ACCOUNTTYPE));
    itemAcctType->Enable(false);

    wxChoice* choice = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS);
    choice->SetSelection(Model_Account::status(m_account));

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT);
    itemCheckBox->SetValue(Model_Account::FAVORITEACCT(m_account));

    Model_Account::currency(m_account);
    wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_NEWACCT_BUTTON_CURRENCY);
    bn->SetLabelText(Model_Account::currency(m_account)->CURRENCYNAME);
    m_currencyID = m_account->CURRENCYID;

    double initBal = m_account->INITIALBAL;
    m_initbalance_ctrl->SetValue(Model_Currency::toString(initBal, Model_Account::currency(m_account)));

    int selectedImage = Option::instance().AccountImageId(m_account->ACCOUNTID);
    m_bitmapButtons->SetBitmap(m_imageList->GetBitmap(selectedImage));

    m_accessInfo = m_account->ACCESSINFO;

    m_credit_limit_ctrl->SetValue(m_account->CREDITLIMIT, 2);
    m_interest_rate_ctrl->SetValue(m_account->INTERESTRATE, 2);

    m_payment_due_date_ctrl->SetValue(Model_Account::DateOf(m_account->PAYMENTDUEDATE));
    m_minimum_payment_ctrl->SetValue(m_account->MINIMUMPAYMENT, 2);

    m_statement_lock_ctrl->SetValue(Model_Account::BoolOf(m_account->STATEMENTLOCKED));
    m_statement_date_ctrl->SetValue(Model_Account::DateOf(m_account->STATEMENTDATE));
    m_minimum_balance_ctrl->SetValue(m_account->MINIMUMBALANCE, 2);

    if (Model_Account::type(m_account) == Model_Account::CREDIT_CARD)
    {
        Model_CreditCard::Data *card = Model_CreditCard::instance().get(m_account->ACCOUNTID);
        Model_Billsdeposits::Data *bdCard = NULL;
        wxChoice* itemHeldAcct = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_HELDACCT);
        wxChoice* itemCardType = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_CARDTYPE);
        wxTextCtrl* itemCardLimit = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDLIMIT);
        wxTextCtrl* itemCardRate = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDINTERESTRATE);
        wxCheckBox* itemCardAutoBal = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCE);
        wxDatePickerCtrl* itemCardAutoBalDate = (wxDatePickerCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEDATE);
        wxTextCtrl* itemCardAutoBalAmnt = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEAMNT);
        if (card)
        {
            bdCard = Model_Billsdeposits::instance().get(Model_CreditCard::getRepeatingTransaction(card->ACCOUNTID, card->HELDAT));
            for (unsigned int i = 0; i < itemHeldAcct->GetCount(); i++)
            {
                if ((int)itemHeldAcct->GetClientData(i) == card->HELDAT)
                {
                    itemHeldAcct->SetSelection(i);
                    break;
                }
            }
            itemCardType->SetSelection(card->CARDTYPE);
            itemCardLimit->SetValue(Model_Currency::toString(card->CARDLIMIT, Model_Account::currency(m_account)));
            itemCardRate->SetValue(Model_Currency::toString(card->RATE, Model_Account::currency(m_account)));
        }
        else
        {
            bdCard = Model_Billsdeposits::instance().get(Model_CreditCard::getRepeatingTransaction(m_account->ACCOUNTID));
            itemCardType->SetSelection(Model_CreditCard::CHARGEALL);
            itemCardLimit->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
            itemCardRate->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
            itemCardAutoBalAmnt->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        }
        if (bdCard)
        {
            itemCardAutoBalDate->SetValue(Model_Billsdeposits::NEXTOCCURRENCEDATE(bdCard));
            itemCardAutoBalAmnt->SetValue(Model_Currency::toString(bdCard->TRANSAMOUNT, Model_Account::currency(m_account)));
        }
        itemCardAutoBal->SetValue(bdCard ? true : false);
        EnableCreditCardFields();
    }
}

void mmNewAcctDialog::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, g_flagsExpand);

    wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(0, 2, 0, 0);
    grid_sizer->AddGrowableCol(1, 1);
    itemBoxSizer3->Add(grid_sizer, g_flagsExpand);

    grid_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Account Name:")), g_flagsH);

    m_textAccountName = new wxTextCtrl(this, wxID_ANY, "");
    grid_sizer->Add(m_textAccountName, g_flagsExpand);

    grid_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Account Type:")), g_flagsH);

    wxChoice* itemChoice61 = new wxChoice(this, ID_DIALOG_NEWACCT_COMBO_ACCTTYPE);
    for (const auto& type : Model_Account::all_type())
        itemChoice61->Append(wxGetTranslation(type), new wxStringClientData(type));
    if (Model_Account::all_type().Index(m_account->ACCOUNTTYPE) == wxNOT_FOUND)
        itemChoice61->Append(m_account->ACCOUNTTYPE);

    grid_sizer->Add(itemChoice61, g_flagsExpand);
    itemChoice61->SetSelection(0);

    grid_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Account Status:")), g_flagsH);

    wxChoice* itemChoice6 = new wxChoice(this, ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS);
    for (const auto& status : Model_Account::all_status())
        itemChoice6->Append(wxGetTranslation(status), new wxStringClientData(status));
    grid_sizer->Add(itemChoice6, g_flagsExpand);
    itemChoice6->SetSelection(0);

    grid_sizer->Add(new wxStaticText(this, wxID_STATIC
        , wxString::Format(_("Initial Balance: %s"), "")), g_flagsH);

    m_initbalance_ctrl = new mmTextCtrl(this
        , ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE
        , "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, mmCalcValidator());
    grid_sizer->Add(m_initbalance_ctrl, g_flagsExpand);

    grid_sizer->Add(new wxStaticText(this, wxID_STATIC, _("Currency:")), g_flagsH);

    wxString currName = _("Select Currency");
    Model_Currency::Data* base_currency = Model_Currency::GetBaseCurrency();
    if (base_currency)
        currName = base_currency->CURRENCYNAME;

    wxButton* itemButton71 = new wxButton(this,
        ID_DIALOG_NEWACCT_BUTTON_CURRENCY, currName);
    grid_sizer->Add(itemButton71, g_flagsExpand);

    wxCheckBox* itemCheckBox10 = new wxCheckBox(this
        , ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT
        , _("Favorite Account"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    itemCheckBox10->SetValue(TRUE);
    grid_sizer->AddSpacer(1);
    grid_sizer->Add(itemCheckBox10, g_flagsH);

    // Notes  ---------------------------------------------

    wxNotebook* acc_notebook = new wxNotebook(this
        , wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE);
    wxPanel* notes_tab = new wxPanel(acc_notebook, wxID_ANY);
    acc_notebook->AddPage(notes_tab, _("Notes"));
    wxBoxSizer *notes_sizer = new wxBoxSizer(wxVERTICAL);
    notes_tab->SetSizer(notes_sizer);

    m_notesCtrl = new wxTextCtrl(notes_tab, ID_DIALOG_NEWACCT_TEXTCTRL_NOTES, ""
        , wxDefaultPosition, wxSize(270, 180), wxTE_MULTILINE);
    notes_sizer->Add(m_notesCtrl, g_flagsExpand);

    // Credit card data

    if (Model_Account::type(m_account) == Model_Account::CREDIT_CARD)
    {
        wxPanel* card_tab = new wxPanel(acc_notebook, wxID_ANY);
        acc_notebook->AddPage(card_tab, _("Credit Card"));
        wxBoxSizer *card_sizer = new wxBoxSizer(wxVERTICAL);
        card_tab->SetSizer(card_sizer);

        wxFlexGridSizer* grid_sizer3 = new wxFlexGridSizer(0, 2, 0, 0);
        grid_sizer3->AddGrowableCol(1, 1);
        card_sizer->Add(grid_sizer3, g_flagsExpand);

        grid_sizer3->Add(new wxStaticText(card_tab, wxID_STATIC, _("Held at:")), g_flagsH);

        wxChoice* itemChoice7 = new wxChoice(card_tab, ID_DIALOG_NEWACCT_COMBO_HELDACCT);
        for (const auto& acct : Model_Account::instance().find(Model_Account::ACCOUNTTYPE(Model_Account::all_type()[Model_Account::CHECKING])))
            itemChoice7->Append(acct.ACCOUNTNAME, (void*)acct.id());
        //itemChoice7->Connect(ID_DIALOG_NEWACCT_COMBO_HELDACCT, wxEVT_COMMAND_COMBOBOX_SELECTED,
        //    wxCommandEventHandler(mmNewAcctDialog::OnCheckCreditCard), nullptr, this);
        itemChoice7->SetToolTip(_("Specify the bank account where credit card is helded."));
        grid_sizer3->Add(itemChoice7, g_flagsExpand);

        grid_sizer3->Add(new wxStaticText(card_tab, wxID_STATIC, _("Card Type:")), g_flagsH);

        wxChoice* itemChoice8 = new wxChoice(card_tab, ID_DIALOG_NEWACCT_COMBO_CARDTYPE);
        for (const auto& type : Model_CreditCard::all_type())
            itemChoice8->Append(wxGetTranslation(type), new wxStringClientData(type));
        //itemChoice8->Connect(ID_DIALOG_NEWACCT_COMBO_CARDTYPE, wxEVT_COMMAND_COMBOBOX_SELECTED,
        //    wxCommandEventHandler(mmNewAcctDialog::OnCheckCreditCard), nullptr, this);
        itemChoice8->SetToolTip(_("Specify the credit card type."));
        grid_sizer3->Add(itemChoice8, g_flagsExpand);

        grid_sizer3->Add(new wxStaticText(card_tab, wxID_STATIC, _("Card Limit:")), g_flagsH);

        wxTextCtrl* itemTextCtrl9 = new wxTextCtrl(card_tab
            , ID_DIALOG_NEWACCT_TEXTCTRL_CARDLIMIT, "", wxDefaultPosition, wxDefaultSize, 0, mmCalcValidator());
        itemTextCtrl9->SetToolTip(_("Enter the credit card limit. This is used to signal limit exceed when inserting transactions."));
        grid_sizer3->Add(itemTextCtrl9, g_flagsExpand);

        grid_sizer3->Add(new wxStaticText(card_tab, wxID_STATIC, _("Interest Rate:")), g_flagsH);

        wxTextCtrl* itemTextCtrl10 = new wxTextCtrl(card_tab
            , ID_DIALOG_NEWACCT_TEXTCTRL_CARDINTERESTRATE, "", wxDefaultPosition, wxDefaultSize, 0, mmCalcValidator());
        itemTextCtrl10->SetToolTip(_("Enter the credit card interest rate. This is used only for credit cards with partial monthly refund."));
        grid_sizer3->Add(itemTextCtrl10, g_flagsExpand);

        wxCheckBox* itemCheckBox11 = new wxCheckBox(card_tab
            , ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCE
            , _("Auto Balance"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
        itemCheckBox11->SetToolTip(_("Specify if you want to insert a repeating transaction for monthly refund. This is used only for credit cards."));
        grid_sizer3->AddSpacer(1);
        grid_sizer3->Add(itemCheckBox11, g_flagsH);

        grid_sizer3->Add(new wxStaticText(card_tab, wxID_STATIC, _("Balance Date")), g_flagsH);

        wxDatePickerCtrl *itemDate1 = new wxDatePickerCtrl(card_tab, ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEDATE
            , wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN | wxDP_SHOWCENTURY);
        grid_sizer3->Add(itemDate1, g_flagsH);
        itemDate1->SetToolTip(_("Specify the balance date of the credit card"));

        grid_sizer3->Add(new wxStaticText(card_tab, wxID_STATIC, _("Balance amount:")), g_flagsH);

        wxTextCtrl* itemTextCtrl11 = new wxTextCtrl(card_tab
            , ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEAMNT, "", wxDefaultPosition, wxDefaultSize, 0, mmCalcValidator());
        itemTextCtrl11->SetToolTip(_("Enter the balance amount of the credit card."));
        grid_sizer3->Add(itemTextCtrl11, g_flagsExpand);
    }

    //

    wxPanel* others_tab = new wxPanel(acc_notebook, wxID_ANY);
    acc_notebook->AddPage(others_tab, _("Others"));
    wxBoxSizer *others_sizer = new wxBoxSizer(wxVERTICAL);
    others_tab->SetSizer(others_sizer);

    wxFlexGridSizer* grid_sizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    grid_sizer2->AddGrowableCol(1, 1);
    others_sizer->Add(grid_sizer2, g_flagsExpand);

    grid_sizer2->Add(new wxStaticText(others_tab, wxID_STATIC
        , (Model_Account::type(m_account) == Model_Account::CREDIT_CARD ? _("Card Number:") : _("Account Number:"))), g_flagsH);

    wxTextCtrl* itemTextCtrl6 = new wxTextCtrl(others_tab
        , ID_ACCTNUMBER, "", wxDefaultPosition, wxDefaultSize);
    grid_sizer2->Add(itemTextCtrl6, g_flagsExpand);

    grid_sizer2->Add(new wxStaticText(others_tab, wxID_STATIC, _("Held At:")), g_flagsH);

    wxTextCtrl* itemTextCtrl8 = new wxTextCtrl(others_tab
        , ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT, "");
    grid_sizer2->Add(itemTextCtrl8, g_flagsExpand);

    grid_sizer2->Add(new wxStaticText(others_tab, wxID_STATIC, _("Website:")), g_flagsH);
    wxTextCtrl* itemTextCtrl10 = new wxTextCtrl(others_tab
        , ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE, "");
    grid_sizer2->Add(itemTextCtrl10, g_flagsExpand);

    grid_sizer2->Add(new wxStaticText(others_tab
        , wxID_STATIC, _("Contact:")), g_flagsH);
    wxTextCtrl* itemTextCtrl12 = new wxTextCtrl(others_tab
        , ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT, "");
    grid_sizer2->Add(itemTextCtrl12, g_flagsExpand);

    grid_sizer2->Add(new wxStaticText(others_tab
        , wxID_STATIC, _("Access Info:")), g_flagsH);
    wxTextCtrl* itemTextCtrl14 = new wxTextCtrl(others_tab
        , ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO, "********************");
    grid_sizer2->Add(itemTextCtrl14, g_flagsExpand);

    //-------------------------------------------------------------------------------------
    wxPanel* statement_tab = new wxPanel(acc_notebook, wxID_ANY);
    acc_notebook->AddPage(statement_tab, _("Statement"));
    wxBoxSizer* statement_sizer = new wxBoxSizer(wxVERTICAL);
    statement_tab->SetSizer(statement_sizer);

    wxFlexGridSizer* statement_grid_sizer = new wxFlexGridSizer(0, 2, 0, 0);
    statement_grid_sizer->AddGrowableCol(1, 1);
    statement_sizer->Add(statement_grid_sizer, g_flagsExpand);

    statement_grid_sizer->Add(new wxStaticText(statement_tab, wxID_STATIC, _("Statement:")), g_flagsH);
    m_statement_lock_ctrl = new wxCheckBox(statement_tab, wxID_ANY, _("Locked")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    statement_grid_sizer->Add(m_statement_lock_ctrl, g_flagsExpand);

    statement_grid_sizer->Add(new wxStaticText(statement_tab, wxID_STATIC, _("Reconciled Date:")), g_flagsH);
    m_statement_date_ctrl = new wxDatePickerCtrl(statement_tab, wxID_ANY, wxDefaultDateTime
        , wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    statement_grid_sizer->Add(m_statement_date_ctrl, g_flagsExpand);

    statement_grid_sizer->Add(new wxStaticText(statement_tab, wxID_STATIC, _("Minimum Bal:")), g_flagsH);
    m_minimum_balance_ctrl = new mmTextCtrl(statement_tab, wxID_ANY, "0.00"
        , wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, mmCalcValidator());
    statement_grid_sizer->Add(m_minimum_balance_ctrl, g_flagsExpand);

    //-------------------------------------------------------------------------------------
    wxPanel* credit_tab = new wxPanel(acc_notebook, wxID_ANY);
    acc_notebook->AddPage(credit_tab, _("Credit"));
    wxBoxSizer* credit_sizer = new wxBoxSizer(wxVERTICAL);
    credit_tab->SetSizer(credit_sizer);

    wxFlexGridSizer* credit_grid_sizer = new wxFlexGridSizer(0, 2, 0, 0);
    credit_grid_sizer->AddGrowableCol(1, 1);
    credit_sizer->Add(credit_grid_sizer, g_flagsExpand);

    credit_grid_sizer->Add(new wxStaticText(credit_tab, wxID_STATIC, _("Credit Limit:")), g_flagsH);
    m_credit_limit_ctrl = new mmTextCtrl(credit_tab, wxID_ANY,""
        , wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, mmCalcValidator());
    credit_grid_sizer->Add(m_credit_limit_ctrl, g_flagsExpand);

    credit_grid_sizer->Add(new wxStaticText(credit_tab, wxID_STATIC, _("Interest Rate:")), g_flagsH);
    m_interest_rate_ctrl = new mmTextCtrl(credit_tab, wxID_ANY, "");
    credit_grid_sizer->Add(m_interest_rate_ctrl, g_flagsExpand);

    credit_grid_sizer->Add(new wxStaticText(credit_tab, wxID_STATIC, _("Payment Due Date:")), g_flagsH);
    m_payment_due_date_ctrl = new wxDatePickerCtrl(credit_tab, wxID_ANY, wxDefaultDateTime
        , wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    credit_grid_sizer->Add(m_payment_due_date_ctrl, g_flagsExpand);

    credit_grid_sizer->Add(new wxStaticText(credit_tab, wxID_STATIC, _("Minimum Payment:")), g_flagsH);
    m_minimum_payment_ctrl = new mmTextCtrl(credit_tab, wxID_ANY, "");
    credit_grid_sizer->Add(m_minimum_payment_ctrl, g_flagsExpand);
    //-------------------------------------------------------------------------------------

    itemBoxSizer3->Add(acc_notebook);

    //Buttons
    wxPanel* itemPanel27 = new wxPanel(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer3->Add(itemPanel27, g_flagsV);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel27->SetSizer(itemBoxSizer28);

    m_bitmapButtons = new wxBitmapButton(itemPanel27
        , wxID_STATIC, wxNullBitmap);
    m_bitmapButtons->Connect(wxID_STATIC, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmNewAcctDialog::OnImageButton), nullptr, this);
    itemBoxSizer28->Add(m_bitmapButtons, g_flagsH);

    bAttachments_ = new wxBitmapButton(itemPanel27, wxID_FILE
        , mmBitmap(png::CLIP));
    bAttachments_->SetToolTip(_("Organize attachments of this account"));
    itemBoxSizer28->Add(bAttachments_, g_flagsH);

    itemBoxSizer28->AddSpacer(20);

    wxButton* itemButton29 = new wxButton(itemPanel27, wxID_OK, _("&OK "));
    itemBoxSizer28->Add(itemButton29, g_flagsH);

    wxButton* itemButton30 = new wxButton(itemPanel27, wxID_CANCEL, wxGetTranslation(g_CancelLabel));
    itemBoxSizer28->Add(itemButton30, g_flagsH);

    if (true)
    {
        m_textAccountName->SetToolTip(_("Enter the Name of the Account. This name can be renamed at any time."));
        itemChoice61->SetToolTip(_("Specify the type of account to be created."));
        itemChoice6->SetToolTip(_("Specify if this account has been closed. Closed accounts are inactive in most calculations, reporting etc."));
        m_initbalance_ctrl->SetToolTip(_("Enter the initial balance in this account."));
        itemButton71->SetToolTip(_("Specify the currency to be used by this account."));
        itemCheckBox10->SetToolTip(_("Select whether this is an account that is used often. This is used to filter accounts display view."));
        m_notesCtrl->SetToolTip(_("Enter user notes and details about this account."));
        itemTextCtrl6->SetToolTip(_("Enter the Account Number associated with this account."));
        itemTextCtrl8->SetToolTip(_("Enter the name of the financial institution in which the account is held."));
        itemTextCtrl10->SetToolTip(_("Enter the URL of the website for the financial institution."));
        itemTextCtrl12->SetToolTip(_("Enter any contact information for the financial institution."));
        itemTextCtrl14->SetToolTip(_("Enter any login/access information for the financial institution. This is not secure as anyone with access to the mmb file can access it."));
        
        m_statement_lock_ctrl->SetToolTip(_("Enable or disable the transaction Lock"));
        m_statement_date_ctrl->SetToolTip(_("The date of the transaction lock"));
        m_minimum_balance_ctrl->SetToolTip(_("Account balance lower limit. Zero to disable"));
        m_credit_limit_ctrl->SetToolTip(_("Credit limit for the Account. Zero to disable"));
    }
}

void mmNewAcctDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void mmNewAcctDialog::OnCurrency(wxCommandEvent& /*event*/)
{
    if (mmMainCurrencyDialog::Execute(this, m_currencyID))
    {
        Model_Currency::Data* currency = Model_Currency::instance().get(m_currencyID);
        wxButton* bn = (wxButton*)FindWindow(ID_DIALOG_NEWACCT_BUTTON_CURRENCY);
        bn->SetLabelText(currency->CURRENCYNAME);

        double init_balance;
        if (m_initbalance_ctrl->checkValue(init_balance, false))
        {
            m_initbalance_ctrl->SetValue(Model_Currency::toString(init_balance, currency));
        }

        if (this->m_account)
        {
            m_account->CURRENCYID = currency->CURRENCYID;
        }
    }
}

void mmNewAcctDialog::OnAttachments(wxCommandEvent& /*event*/)
{
    wxString RefType = Model_Attachment::reftype_desc(Model_Attachment::BANKACCOUNT);
    mmAttachmentDialog dlg(this, RefType, m_account->ACCOUNTID);
    dlg.ShowModal();
}

void mmNewAcctDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString acctName = m_textAccountName->GetValue().Trim();
    if (acctName.IsEmpty() || Model_Account::Exist(acctName))
    {
        if (m_account && m_account->ACCOUNTNAME != acctName)
            return mmErrorDialogs::MessageInvalid(this, _("Account Name "));
    }

    if (m_currencyID == -1)
        return mmErrorDialogs::MessageInvalid(this, _("Currency"));

    Model_Currency::Data* currency = Model_Currency::instance().get(m_currencyID);
    m_initbalance_ctrl->Calculate(Model_Currency::precision(currency));
    if (!m_initbalance_ctrl->checkValue(m_account->INITIALBAL, false))
        return;

    if (!this->m_account) this->m_account = Model_Account::instance().create();

    wxTextCtrl* textCtrlAcctNumber = (wxTextCtrl*)FindWindow(ID_ACCTNUMBER);
    wxTextCtrl* textCtrlHeldAt = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_HELDAT);
    wxTextCtrl* textCtrlWebsite = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_WEBSITE);
    wxTextCtrl* textCtrlContact = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CONTACT);

    wxChoice* choice = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_ACCTSTATUS);
    m_account->STATUS = Model_Account::all_status()[choice->GetSelection()];

    wxCheckBox* itemCheckBox = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_CHKBOX_FAVACCOUNT);
    m_account->FAVORITEACCT = itemCheckBox->IsChecked() ? "TRUE" : "FALSE";

    m_account->ACCOUNTNAME = acctName;
    m_account->ACCOUNTNUM = textCtrlAcctNumber->GetValue();
    m_account->NOTES = m_notesCtrl->GetValue();
    m_account->HELDAT = textCtrlHeldAt->GetValue();
    m_account->WEBSITE = textCtrlWebsite->GetValue();
    m_account->CONTACTINFO = textCtrlContact->GetValue();
    m_account->CURRENCYID = m_currencyID;
    m_account->ACCESSINFO = m_accessInfo;

    double value = 0;
    m_credit_limit_ctrl->checkValue(value);
    m_account->CREDITLIMIT = value;
    
    m_interest_rate_ctrl->checkValue(value);
    m_account->INTERESTRATE = value;

    m_account->PAYMENTDUEDATE = m_payment_due_date_ctrl->GetValue().FormatISODate();

    m_minimum_payment_ctrl->checkValue(value);
    m_account->MINIMUMPAYMENT = value;

    m_account->STATEMENTLOCKED = m_statement_lock_ctrl->GetValue() ? 1 : 0;
    m_account->STATEMENTDATE = m_statement_date_ctrl->GetValue().FormatISODate();

    m_minimum_balance_ctrl->checkValue(value);
    m_account->MINIMUMBALANCE = value;

    Model_Account::instance().save(m_account);

    if (Model_Account::type(m_account) == Model_Account::CREDIT_CARD)
    {
        wxChoice* itemHeldAcct = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_HELDACCT);
        wxChoice* itemCardType = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_CARDTYPE);
        wxTextCtrl* itemCardLimit = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDLIMIT);
        wxTextCtrl* itemCardRate = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDINTERESTRATE);
        wxCheckBox* itemCardAutoBal = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCE);
        wxDatePickerCtrl* itemCardAutoBalDate = (wxDatePickerCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEDATE);
        wxTextCtrl* itemCardAutoBalAmnt = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEAMNT);
        Model_CreditCard::Data *card = Model_CreditCard::instance().get(m_account->ACCOUNTID);
        if (!card)
            card = Model_CreditCard::instance().create();
        card->ACCOUNTID = m_account->ACCOUNTID;
        int curSel = itemHeldAcct->GetCurrentSelection();
        if (curSel >= 0)
            card->HELDAT = (int)itemHeldAcct->GetClientData(curSel);
        else
            card->HELDAT = -1;
        card->CARDTYPE = itemCardType->GetSelection();
        wxString value = itemCardLimit->GetValue().Trim();
        if (!Model_Currency::fromString(value, card->CARDLIMIT, Model_Currency::instance().get(m_currencyID)))
            card->CARDLIMIT = 0.0;
        value = itemCardRate->GetValue().Trim();
        if (!Model_Currency::fromString(value, card->RATE, Model_Currency::instance().get(m_currencyID)))
            card->RATE = 0.0;

        Model_CreditCard::instance().save(card);

        // auto balance in repeating transactions
        Model_Billsdeposits::Data *bdCard = Model_Billsdeposits::instance().get(Model_CreditCard::getRepeatingTransaction(card->ACCOUNTID, card->HELDAT));
        if (itemCardAutoBal->GetValue())
        {
            double balanceAmount = 0.0;
            wxDate balanceDate = itemCardAutoBalDate->GetValue();
            value = itemCardAutoBalAmnt->GetValue().Trim();
            if (!Model_Currency::fromString(value, balanceAmount, Model_Currency::instance().get(m_currencyID)))
                balanceAmount = 0.0;
            if (!bdCard)
                bdCard = Model_Billsdeposits::instance().create();
            bdCard->ACCOUNTID = card->HELDAT;
            bdCard->TOACCOUNTID = card->ACCOUNTID;
            bdCard->PAYEEID = card->HELDAT;
            bdCard->TRANSCODE = Model_Billsdeposits::all_type()[Model_Billsdeposits::TRANSFER];
            bdCard->TRANSAMOUNT = bdCard->TOTRANSAMOUNT = balanceAmount;
            Model_Category::Data_Set category = Model_Category::instance().find(Model_Category::CATEGNAME(_("Credit Card")));
            if (category.empty())
            {
                Model_Category::Data *newCat = Model_Category::instance().create();
                newCat->CATEGNAME = _("Credit Card");
                bdCard->CATEGID = Model_Category::instance().save(newCat);
            }
            else
                bdCard->CATEGID = category[0].id();
            if (bdCard->NEXTOCCURRENCEDATE != balanceDate.FormatISODate())
            {
                bdCard->TRANSDATE = bdCard->NEXTOCCURRENCEDATE = balanceDate.FormatISODate();
                if (balanceDate < wxDate::Today())
                    bdCard->NEXTOCCURRENCEDATE = Model_Billsdeposits::instance().nextOccurDate(Model_Billsdeposits::REPEAT_MONTHLY, 1, balanceDate).FormatISODate();
            }
            bdCard->REPEATS = BD_REPEATS_MULTIPLEX_BASE + Model_Billsdeposits::REPEAT_MONTHLY;
            bdCard->NUMOCCURRENCES = -1;
            Model_Billsdeposits::instance().save(bdCard);
        }
        else if (bdCard)
            Model_Billsdeposits::instance().remove(bdCard->id());
    }

    EndModal(wxID_OK);
    mmWebApp::MMEX_WebApp_UpdateAccount();
}

void mmNewAcctDialog::OnImageButton(wxCommandEvent& /*event*/)
{
    wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, wxID_ANY);
    ev.SetEventObject(this);

    wxMenu* mainMenu = new wxMenu;
    wxMenuItem* menuItem = new wxMenuItem(mainMenu, wxID_HIGHEST + acc_img::MONEY_DOLLAR_XPM - 1, _("Default Image"));
    menuItem->SetBitmap(m_imageList->GetBitmap(Option::instance().AccountImageId(this->m_account->ACCOUNTID, true)));
    mainMenu->Append(menuItem);

    for (int i = img::LAST_NAVTREE_PNG; i < acc_img::MAX_XPM; ++i)
    {
        menuItem = new wxMenuItem(mainMenu, wxID_HIGHEST + i
            , wxString::Format(_("Image #%i"), i - img::LAST_NAVTREE_PNG + 1));
        menuItem->SetBitmap(m_imageList->GetBitmap(i));
        mainMenu->Append(menuItem);
    }

    PopupMenu(mainMenu);
    delete mainMenu;
}

void mmNewAcctDialog::OnCustonImage(wxCommandEvent& event)
{
    int selectedImage = (event.GetId() - wxID_HIGHEST) - img::LAST_NAVTREE_PNG + 1;
    int image_id = Option::instance().AccountImageId(this->m_account->ACCOUNTID, true);

    Model_Infotable::instance().Set(wxString::Format("ACC_IMAGE_ID_%i", this->m_account->ACCOUNTID)
        , selectedImage);
    if (selectedImage != 0)
        image_id = selectedImage + img::LAST_NAVTREE_PNG - 1;

    m_bitmapButtons->SetBitmap(m_imageList->GetBitmap(image_id));
}

void mmNewAcctDialog::OnChangeFocus(wxChildFocusEvent& event)
{
    wxWindow *w = event.GetWindow();
    int oject_in_focus = 0;
    if ( w ) oject_in_focus = w->GetId();

    wxTextCtrl* textCtrl = (wxTextCtrl*) FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO);
    if (oject_in_focus == ID_DIALOG_NEWACCT_TEXTCTRL_ACCESSINFO)
    {
        if (!m_accessinfo_infocus)
        {
            textCtrl->SetValue(m_accessInfo);
            m_accessinfo_infocus = true;
        }
    }
    else
    {
        if (m_accessinfo_infocus)
        {
            m_accessInfo = textCtrl->GetValue();
            textCtrl->SetValue("********************");
            m_accessinfo_infocus = false;
        }
    }
}

void mmNewAcctDialog::OnTextEntered(wxCommandEvent& event)
{
    if (event.GetId() == m_initbalance_ctrl->GetId())
    {
        Model_Currency::Data* currency = Model_Currency::instance().get(m_currencyID);
        if (!currency)
        {
            currency = Model_Currency::GetBaseCurrency();
        }
        m_initbalance_ctrl->Calculate(Model_Currency::precision(currency));
    }
}

void mmNewAcctDialog::OnCreditCardFields(wxCommandEvent& event)
{
    EnableCreditCardFields();
}

void mmNewAcctDialog::EnableCreditCardFields()
{
    wxChoice* itemHeldAcct = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_HELDACCT);
    wxChoice* itemCardType = (wxChoice*)FindWindow(ID_DIALOG_NEWACCT_COMBO_CARDTYPE);
    wxTextCtrl* itemCardLimit = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDLIMIT);
    wxTextCtrl* itemCardRate = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDINTERESTRATE);
    wxCheckBox* itemCardAutoBal = (wxCheckBox*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCE);
    wxDatePickerCtrl* itemCardAutoBalDate = (wxDatePickerCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEDATE);
    wxTextCtrl* itemCardAutoBalAmnt = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_CARDAUTOBALANCEAMNT);

    // enable/disable field based on options
    itemCardLimit->Enable(true);
    itemCardRate->Enable(true);
    itemCardAutoBal->Enable(true);
    itemCardAutoBalDate->Enable(true);
    itemCardAutoBalAmnt->Enable(true);
    if (itemHeldAcct->GetSelection() == -1)
    {
        itemCardAutoBal->SetValue(false);
        itemCardAutoBal->Enable(false);
        itemCardAutoBalDate->Enable(false);
        itemCardAutoBalAmnt->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardAutoBalAmnt->Enable(false);
    }
    switch (itemCardType->GetSelection())
    {
    case Model_CreditCard::NOCHARGE:
        itemCardLimit->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardLimit->Enable(false);
        itemCardRate->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardRate->Enable(false);
        itemCardAutoBal->SetValue(false);
        itemCardAutoBal->Enable(false);
        itemCardAutoBalDate->Enable(false);
        itemCardAutoBalAmnt->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardAutoBalAmnt->Enable(false);
        break;
    case Model_CreditCard::CHARGEPARTLY:
        break;
    case Model_CreditCard::CHARGEALL:
        itemCardRate->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardRate->Enable(false);
        itemCardAutoBalAmnt->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardAutoBalAmnt->Enable(false);
        break;
    }
    if (!itemCardAutoBal->GetValue())
    {
        itemCardAutoBalDate->Enable(false);
        itemCardAutoBalAmnt->SetValue(Model_Currency::toString(0.0, Model_Account::currency(m_account)));
        itemCardAutoBalAmnt->Enable(false);
    }
}
