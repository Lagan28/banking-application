#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

class Account
{
private:
    string accountNumber;
    string accountHolderName;
    double balance;

public:
    Account(string number, string name, double initialBalance)
    {
        accountNumber = number;
        accountHolderName = name;
        balance = initialBalance;
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
};

class Bank
{
private:
    Account **accounts;
    int numAccounts;
    const double loanInterestRate = 5.0;

public:
    Bank()
    {
        accounts = nullptr;
        numAccounts = 0;
    }

    ~Bank()
    {
        for (int i = 0; i < numAccounts; i++)
        {
            delete accounts[i];
        }
        delete[] accounts;
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
        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == accountNumber)
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

        Account **temp = new Account *[numAccounts + 1];
        for (int i = 0; i < numAccounts; i++)
        {
            temp[i] = accounts[i];
        }

        temp[numAccounts] = account;

        delete[] accounts;
        accounts = temp;
        numAccounts++;

        cout << "Account created successfully!" << endl;
    }

    void closeAccount()
    {
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == accountNumber)
            {
                delete accounts[i];
                numAccounts--;

                Account **temp = new Account *[numAccounts];
                for (int j = 0, k = 0; j < numAccounts + 1; j++)
                {
                    if (j != i)
                    {
                        temp[k++] = accounts[j];
                    }
                }

                delete[] accounts;
                accounts = temp;

                cout << "Account closed successfully!" << endl;
                return;
            }
        }

        throw "Account not found";
    }

    void displayAccountInfo()
    {
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == accountNumber)
            {
                cout << "Account Number: " << accounts[i]->getAccountNumber() << endl;
                cout << "Account Holder Name: " << accounts[i]->getAccountHolderName() << endl;
                cout << "Balance: " << accounts[i]->getBalance() << endl;
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

        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == senderAccountNumber)
            {
                for (int j = 0; j < numAccounts; j++)
                {
                    if (accounts[j]->getAccountNumber() == receiverAccountNumber)
                    {
                        cout << "Enter amount to transfer: ";
                        cin >> amount;

                        if (accounts[i]->getBalance() >= amount)
                        {
                            accounts[i]->withdraw(amount);
                            accounts[j]->deposit(amount);
                            cout << "Amount transferred successfully!" << endl;
                            return;
                        }
                        else
                        {
                            throw "Insufficient balance";
                        }
                    }
                }

                throw "Receiver account not found";
            }
        }

        throw "Sender account not found";
    }

    void checkBalance()
    {
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == accountNumber)
            {
                cout << "Balance: " << accounts[i]->getBalance() << endl;
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

        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == accountNumber)
            {
                cout << "Enter amount to withdraw: ";
                cin >> amount;

                accounts[i]->withdraw(amount);
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

        for (int i = 0; i < numAccounts; i++)
        {
            if (accounts[i]->getAccountNumber() == accountNumber)
            {
                cout << "Enter loan amount: ";
                cin >> loanAmount;

                cout << "Enter time period (in years): ";
                cin >> timePeriod;

                double interest = loanAmount * loanInterestRate * timePeriod / 100;
                accounts[i]->deposit(loanAmount);
                cout << "Loan granted successfully!" << endl;
                cout << "Loan Amount: " << loanAmount << endl;
                cout << "Interest Rate: " << loanInterestRate << "%" << endl;
                cout << "Time Period: " << timePeriod << " years" << endl;
                cout << "Total Repayment Amount: " << loanAmount + interest << endl;

                // Add loan information to passbook
                ofstream passbook(accountNumber + ".txt", ios::app);
                passbook << "Loan Granted:" << endl;
                passbook << "Loan Amount: " << loanAmount << endl;
                passbook << "Interest Rate: " << loanInterestRate << "%" << endl;
                passbook << "Time Period: " << timePeriod << " years" << endl;
                passbook << "Total Repayment Amount: " << loanAmount + interest << endl;
                passbook.close();

                return;
            }
        }

        throw "Account not found";
    }

    void printPassbook()
    {
        string accountNumber;
        cout << "Enter account number: ";
        cin >> accountNumber;

        ifstream passbook(accountNumber + ".txt");
        if (passbook)
        {
            string line;
            while (getline(passbook, line))
            {
                cout << line << endl;
            }
            passbook.close();
        }
        else
        {
            throw "Passbook not found";
        }
    }
};

void displayMainMenu()
{
    cout << "=========================" << endl;
    cout << "Banking Application Menu" << endl;
    cout << "=========================" << endl;
    cout << "1. Open Account" << endl;
    cout << "2. Close Account" << endl;
    cout << "3. Display Account Information" << endl;
    cout << "4. Transfer Amount" << endl;
    cout << "5. Check Balance" << endl;
    cout << "6. Withdraw Amount" << endl;
    cout << "7. Grant Loan" << endl;
    cout << "8. Print Passbook" << endl;
    cout << "0. Exit" << endl;
    cout << "=========================" << endl;
    cout << "Enter your choice: ";
}

int main()
{
    Bank bank;
    string username, password;

    // Sign-up
    cout << "Welcome to XYZ Bank" << endl;
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
    int loginAttempts = 3;

    while (loginAttempts > 0)
    {
        cout << "Enter username: ";
        cin >> enteredUsername;

        cout << "Enter password: ";
        cin >> enteredPassword;

        if (enteredUsername == username && enteredPassword == password)
        {
            cout << "Login successful!" << endl;
            break;
        }
        else
        {
            loginAttempts--;
            cout << "Incorrect username or password. " << loginAttempts << " attempts remaining." << endl;
        }
    }

    if (loginAttempts == 0)
    {
        cout << "Maximum login attempts reached. Exiting..." << endl;
        return 0;
    }

    int choice;
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
                bank.displayAccountInfo();
                break;
            case 4:
                bank.transferAmount();
                break;
            case 5:
                bank.checkBalance();
                break;
            case 6:
                bank.withdrawAmount();
                break;
            case 7:
                bank.grantLoan();
                break;
            case 8:
                bank.printPassbook();
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
    }

    return 0;
}
