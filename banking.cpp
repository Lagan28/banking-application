#include <iostream>
#include <fstream>
#include <regex>
#include <vector>

using namespace std;

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

public:
    ~Bank()
    {
        for (auto account : accounts)
        {
            delete account;
        }
    }

    void openAccount()
    {
        string accountNumber, accountHolderName;
        double initialBalance;

        cout << "Enter account number: ";
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
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

        for (auto it = accounts.begin(); it != accounts.end(); ++it)
        {
            if ((*it)->getAccountNumber() == accountNumber)
            {
                delete *it;
                accounts.erase(it);
                cout << "Account closed successfully!" << endl;
                return;
            }
        }

        throw "Account not found";
    }

    void transferAmount()
    {
        string senderAccountNumber, receiverAccountNumber;
        double amount;

        cout << "Enter sender account number: ";
        cin >> senderAccountNumber;

        cout << "Enter receiver account number: ";
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
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

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
        string accountNumber;
        double amount;

        cout << "Enter account number: ";
        cin >> accountNumber;

        for (const auto &account : accounts)
        {
            if (account->getAccountNumber() == accountNumber)
            {
                cout << "Enter amount to withdraw: ";
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
        string accountNumber;
        double loanAmount, timePeriod;

        cout << "Enter account number: ";
        cin >> accountNumber;

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

        cout << "Loan granted successfully!" << endl;
        cout << "Loan Amount: " << loanAmount << endl;
        cout << "Interest Rate: " << loanInterestRate << "%" << endl;
        cout << "Time Period: " << timePeriod << " years" << endl;
        cout << "Total Repayment Amount: " << loanAmount + interest << endl;
    }

    void getPrintPassbook()
    {
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

        ifstream passbookFile(accountNumber + ".txt");
        ofstream passbook;

        if (!passbookFile)
        {
            // Passbook file doesn't exist, create a new one
            passbook.open(accountNumber + ".txt");
            passbook << "Passbook for Account Number: " << accountNumber << endl;
        }
        else
        {
            // Passbook file exists, append to existing file
            passbook.open(accountNumber + ".txt", ios::app);
        }

        if (!passbook)
        {
            throw "Error in accessing the passbook";
        }

        // Get current time
        // auto currentTime = system_clock::to_time_t(system_clock::now());
        // passbook << "\n----- Updated on: " << ctime(&currentTime);

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
        cout << "Passbook printed successfully!" << endl;
    }
};

void displayMainMenu()
{
    cout << "\nMain Menu" << endl;
    cout << "1. Open Account" << endl;
    cout << "2. Close Account" << endl;
    cout << "3. Transfer Amount" << endl;
    cout << "4. Check Balance" << endl;
    cout << "5. Withdraw Amount" << endl;
    cout << "6. Grant Loan" << endl;
    cout << "7. Print Passbook" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter your choice: ";
}

int main()
{
    Bank bank;
    int choice;

    cout << "Welcome to XYZ Bank" << endl;
    cout << "===================" << endl;

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
        return 0;
    }

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
            case 0:
                cout << "Thank you for using XYZ Bank. Goodbye!" << endl;
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
