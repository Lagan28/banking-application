#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <ctime>
#include <chrono>
#include <windows.h>
#include<unistd.h>

using namespace std;
using namespace std::chrono;

//Account Class
class Account
{
private:
    string accountNumber;
    string accountHolderName;
    double balance;
    double initialBalance;

public:
    Account(string number, string name, double initialBalance)
    {
        accountNumber = number;
        accountHolderName = name;
        balance = initialBalance;
        this->initialBalance = initialBalance;
    }

    string getAccountNumber() const
    {
        return accountNumber;
    }

    string getAccountHolderName() const
    {
        return accountHolderName;
    }

    double getBalance() const
    {
        return balance;
    }

    void deposit(double amount)
    {
        balance += amount;
    }

    void withdraw(double amount)
    {
        if (balance >= amount)
        {
            balance -= amount;
        }
        else
        {
            throw "Insufficient balance";
        }
    }

    double getInitialBalance() const
    {
        return initialBalance;
    }
};

class Bank
{
private:
    vector<Account *> accounts;
    const double loanInterestRate = 5.0;
    string selectedAccountNumber;

public:
    ~Bank()
    {
        for (auto account : accounts)
        {
            delete account;
        }
    }

    void selectedAccount();
    string getSelectedAccountNumber() const;

    void openAccount()
    {
        string accountNumber, accountHolderName;
        double initialBalance;

        cout << "\n\nEnter account number: ";
        cin >> accountNumber;

        // Validate account number using regex
        regex accountNumberPattern("[0-9]+");
        if (!regex_match(accountNumber, accountNumberPattern))
        {
            throw "Invalid account number";
        }

        // Check if the account number already exists
        for (const auto &account : accounts)
        {
            if (account->getAccountNumber() == accountNumber)
            {
                throw "Account number already exists";
            }
        }

        cout << "Enter account holder name: ";
        cin.ignore();
        getline(cin, accountHolderName);

        cout << "Enter initial balance: ";
        cin >> initialBalance;

        Account *account = new Account(accountNumber, accountHolderName, initialBalance);
        accounts.push_back(account);

        cout << "Account created successfully!" << endl;
    }

    void closeAccount()
    {
        string accountNumber = selectedAccountNumber;

        for (auto it = accounts.begin(); it != accounts.end(); ++it)
        {
            if ((*it)->getAccountNumber() == accountNumber)
            {
                delete *it;
                accounts.erase(it);
                cout << "\n\nAccount closed successfully!" << endl;
                return;
            }
        }

        throw "Account not found";
    }

    void transferAmount()
    {
        string receiverAccountNumber;
        double amount;

        string senderAccountNumber = selectedAccountNumber;

        cout << "\n\nEnter receiver account number: ";
        cin >> receiverAccountNumber;

        Account *sender = nullptr;
        Account *receiver = nullptr;

        for (const auto &account : accounts)
        {
            if (account->getAccountNumber() == senderAccountNumber)
            {
                sender = account;
            }
            else if (account->getAccountNumber() == receiverAccountNumber)
            {
                receiver = account;
            }

            if (sender && receiver)
            {
                break;
            }
        }

        if (!sender)
        {
            throw "Sender account not found";
        }

        if (!receiver)
        {
            throw "Receiver account not found";
        }

        cout << "Enter amount to transfer: ";
        cin >> amount;

        if (sender->getBalance() >= amount)
        {
            sender->withdraw(amount);
            receiver->deposit(amount);
            cout << "Amount transferred successfully!" << endl;
        }
        else
        {
            throw "Insufficient balance";
        }
    }

    void checkBalance()
    {
        string accountNumber = selectedAccountNumber;
        cout<<"\n\nThe current account number is : " <<accountNumber<<endl;

        for (const auto &account : accounts)
        {
            if (account->getAccountNumber() == accountNumber)
            {
                cout << "Balance: " << account->getBalance() << endl;
                return;
            }
        }

        throw "Account not found";
    }

    void withdrawAmount()
    {
        string accountNumber = selectedAccountNumber;
        double amount;

        for (const auto &account : accounts)
        {
            if (account->getAccountNumber() == accountNumber)
            {
                cout << "\n\nEnter amount to withdraw: ";
                cin >> amount;

                account->withdraw(amount);
                cout << "Amount withdrawn successfully!" << endl;
                return;
            }
        }

        throw "Account not found";
    }

    void grantLoan()
    {
        string accountNumber = selectedAccountNumber;
        double loanAmount, timePeriod;

        Account *account = nullptr;

        for (const auto &acc : accounts)
        {
            if (acc->getAccountNumber() == accountNumber)
            {
                account = acc;
                break;
            }
        }

        if (!account)
        {
            throw "Account not found";
        }

        cout << "Enter loan amount: ";
        cin >> loanAmount;

        cout << "Enter time period (in years): ";
        cin >> timePeriod;

        double interest = loanAmount * loanInterestRate * timePeriod / 100;
        account->deposit(loanAmount);

        cout << "\n\nLoan granted successfully!" << endl;
        cout << "Loan Amount: " << loanAmount << endl;
        cout << "Interest Rate: " << loanInterestRate << "%" << endl;
        cout << "Time Period: " << timePeriod << " years" << endl;
        cout << "Total Repayment Amount: " << loanAmount + interest << endl;
    }

    void getPrintPassbook()
    {
        string accountNumber = selectedAccountNumber;

        ifstream passbookFile(accountNumber + ".txt");
        ofstream passbook;

        // if (!passbookFile)
        // {
        //     // Passbook file doesn't exist, create a new one
        //     passbook.open(accountNumber + ".txt");
        //     passbook << "Passbook for Account Number: " << accountNumber << endl;
        // }
        // else
        // {
        //     // Passbook file exists, append to existing file
        //     passbook.open(accountNumber + ".txt", ios::app);
        // }

        passbook.open(accountNumber + ".txt");

        if (!passbook)
        {
            throw "Error in accessing the passbook";
        }

        //Get current time
        auto currentTime = system_clock::to_time_t(system_clock::now());
        passbook << "\n----- Updated on: " << ctime(&currentTime);

        string line;
        while (getline(passbookFile, line))
        {
            // Display passbook contents from the file
            passbook << line << endl;
        }

        passbookFile.close();

        // Retrieve the latest transactions and append them to the passbook
        for (const auto &account : accounts)
        {
            if (account->getAccountNumber() == accountNumber)
            {
                passbook << "Account Holder Name: " << account->getAccountHolderName() << endl;
                passbook << "Account Balance: " << account->getBalance() << endl;

                // Add loan details if granted
                double loanAmount = account->getBalance() - account->getInitialBalance();
                if (loanAmount > 0)
                {
                    double interest = loanAmount * loanInterestRate / 100;
                    passbook << "Loan Granted: " << loanAmount << " (Interest Rate: " << loanInterestRate << "%)" << endl;
                    passbook << "Total Repayment Amount: " << account->getBalance() << endl;
                }

                // Add more details or transactions as needed
                break;
            }
        }

        passbook.close();
        cout << "\nPassbook printed successfully!" << endl;
    }
};

void Bank::selectedAccount(){
    bool isValidAccount = false;

    while(!isValidAccount){
        cout<<"\nEnter the account number: ";
        cin >> selectedAccountNumber;

        for(const auto&account : accounts){
            if(account->getAccountNumber() == selectedAccountNumber){
                isValidAccount = true;
                break;
            }
        }
        if(!isValidAccount){
            cout<<"Invalid account number. Try again"<<endl;
        }
    }
    cout<<"Account "<<selectedAccountNumber<<" selected."<<endl;
}

string Bank::getSelectedAccountNumber() const{
    return selectedAccountNumber;
}

void displayMainMenu()
{
    cout << "\nWelcome to the BANK. Choose the service you want!" << endl;
    cout << "-------------------------------------------------" << endl;
    cout << "Main Menu" << endl;
    cout << "1. Open Account" << endl;
    cout << "2. Close Account" << endl;
    cout << "3. Transfer Amount" << endl;
    cout << "4. Account Details" << endl;
    cout << "5. Withdraw Amount" << endl;
    cout << "6. Grant Loan" << endl;
    cout << "7. Print Passbook" << endl;
    cout << "8. Change account number" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter your choice: ";
}

int main()
{
    Bank bank;
    int choice;

    cout << "-------------------" << endl;
    cout << "Welcome to the Bank" << endl;
    cout << "-------------------" << endl;

    string username, password;
    bool isLoggedIn = false;
    int loginAttempts = 3;

    // Sign-up
    cout << "Sign Up" << endl;
    cout << "Enter username: ";
    cin >> username;

    // Validate username using regex
    regex usernamePattern("[a-zA-Z0-9]+");
    if (!regex_match(username, usernamePattern))
    {
        cout << "Invalid username. Only alphanumeric characters are allowed." << endl;
        return 0;
    }

    cout << "Enter password: ";
    cin >> password;

    // Validate password using regex
    regex passwordPattern("[a-zA-Z0-9]+");
    if (!regex_match(password, passwordPattern))
    {
        cout << "Invalid password. Only alphanumeric characters are allowed." << endl;
        return 0;
    }

    system("cls");

    // Login
    cout << "Login" << endl;

    string enteredUsername, enteredPassword;

    while (loginAttempts > 0)
    {
        cout << "Enter username: ";
        cin >> enteredUsername;

        cout << "Enter password: ";
        cin >> enteredPassword;

        if (enteredUsername == username && enteredPassword == password)
        {
            cout << "Login successful!" << endl;
            isLoggedIn = true;
            system("cls");
            break;
        }
        else
        {
            loginAttempts--;
            cout << "Incorrect username or password. " << loginAttempts << " attempts remaining." << endl;
        }
    }

    if (!isLoggedIn)
    {
        cout << "Maximum login attempts reached. Exiting..." << endl;
        sleep(3);
        return 0;
    }

    cout<<"First time user? Create an account!"<<endl;
    try{
        bank.openAccount();
    }catch(const char *e){
        cout << "Error " << e << endl;
        return 0;
    }

    bank.selectedAccount();

    while (true)
    {
        displayMainMenu();
        cin >> choice;

        try
        {
            switch (choice)
            {
            case 1:
                bank.openAccount();
                break;
            case 2:
                bank.closeAccount();
                break;
            case 3:
                bank.transferAmount();
                break;
            case 4:
                bank.checkBalance();
                break;
            case 5:
                bank.withdrawAmount();
                break;
            case 6:
                bank.grantLoan();
                break;
            case 7:
                bank.getPrintPassbook();
                break;
            case 8:
                bank.selectedAccount();
                break;
            case 0:
                cout << "Thank you for using the Bank. Goodbye!" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        catch (const char *e)
        {
            cout << "Error: " << e << endl;
        }

        cout << endl;
    }

    return 0;
}
