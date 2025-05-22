#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <ctime>
#include <limits>

// ───────────────────────────── Transaction record
struct Transaction {
    double amount{};          // positive number
    char   type{ 'D' };       // 'D' = Deposit, 'W' = Withdrawal
    std::string timestamp;     // "YYYY-MM-DD HH:MM:SS"
};

// Return local time as "YYYY-MM-DD HH:MM:SS"
std::string currentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm tmLocal{};                 
    localtime_s(&tmLocal, &now);        
    char buf[32];
    std::strftime(buf, sizeof(buf),
        "%Y-%m-%d %H:%M:%S", &tmLocal);
    return buf;
}


// ───────────────────────────── BankAccount class
class BankAccount
{
    
public:
    explicit BankAccount(double startBalance = 500.00,
        
        const std::string& file = "transactions.txt")
        : balance(startBalance), dataFile(file)
    {
        loadHistory();                    // populate vector + adjust balance
    }
    double getBalance() const { return balance; }
    // menu actions -----------------------------------------------------------
    void showBalance() const
    {
        std::cout << std::fixed << std::setprecision(2)
            << "Balance: $" << balance << "\n";
    }

    void makeDeposit()
    {
        double amt;
        std::cout << "Enter deposit amount: $";
        if (!(std::cin >> amt) || amt <= 0) {
            std::cout << "Invalid! Deposit must be positive.\n";
        }
        else {
            balance += amt;
            history.push_back({ amt, 'D', currentTimestamp() });
            std::cout << "Deposited $" << amt << ".\n";
        }
    }

    void makeWithdrawal()
    {
        double amt;
        std::cout << "Enter withdrawal amount: $";
        if (!(std::cin >> amt) || amt <= 0 || amt > balance) {
            std::cout << "Invalid! Must be >0 and <= current balance.\n";
        }
        else {
            balance -= amt;
            history.push_back({ amt, 'W', currentTimestamp() });
            std::cout << "Withdrew $" << amt << ".\n";
        }
    }

    void listDeposits()        const { listByType('D'); }
    void listWithdrawals()     const { listByType('W'); }
    void listAllTransactions() const { listByType('A'); }

    // called once, right before program exits
    void saveHistory() const
    {
        std::ofstream fout(dataFile, std::ios::trunc);
        for (const auto& tr : history)
            fout << tr.type << ' ' << tr.amount << ' ' << tr.timestamp << '\n';
    }

private:
    double balance;
    std::vector<Transaction> history;   // dynamic *array* of all records
    std::string dataFile;

    // helper: load from file and recalc balance
    void loadHistory()
    {
        std::ifstream fin(dataFile);
        if (!fin) return;               // first run: nothing to load

        char   type;
        double amt;
        std::string ts;
        while (fin >> type >> amt) {
            std::getline(fin, ts);      // rest of the line = timestamp
            if (!ts.empty() && ts[0] == ' ') ts.erase(0, 1);
            history.push_back({ amt, type, ts });
            balance += (type == 'D') ? amt : -amt;
        }
    }

    // helper: print transactions filtered by type
    void listByType(char filter) const
    {
        if (history.empty()) {
            std::cout << "(no transactions)\n";
            return;
        }
        std::cout << std::left
            << std::setw(12) << "Type"
            << std::setw(12) << "Amount"
            << "Timestamp\n";
        std::cout << "---------------------------------------------\n";
        for (const auto& tr : history) {
            if (filter != 'A' && tr.type != filter) continue;
            std::cout << std::setw(12)
                << (tr.type == 'D' ? "Deposit" : "Withdrawal")
                << '$' << std::setw(11) << std::fixed << std::setprecision(2)
                << tr.amount
                << tr.timestamp << '\n';
        }
    }
};

// ───────────────────────────── main – user interface loop
int main()
{
    BankAccount account;                       // Load history or start at $500
    double startingBalance = account.getBalance();   // Remember initial balance

    std::string name;
    std::cout << "What is your name? ";
    std::getline(std::cin, name);
    std::cout << "\nHello, " << name
        << " – welcome to your account!\n\n";

    int choice = 0;
    while (choice != 7)                       // Main menu loop
    {
        std::cout << "\n========== MAIN MENU ==========\n";
        account.showBalance();
        std::cout << "\n1) Balance\n"
            << "2) Make a Deposit\n"
            << "3) Make a Withdrawal\n"
            << "4) Deposit History\n"
            << "5) Withdrawal History\n"
            << "6) All Deposits and Withdrawals\n"
            << "7) Exit\n"
            << "Select an option: ";

        if (!(std::cin >> choice)) {          // Non-numeric guard
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a number 1-7.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Flush '\n'

        switch (choice) {
        case 1: account.showBalance();        break;
        case 2: account.makeDeposit();         break;
        case 3: account.makeWithdrawal();      break;
        case 4: account.listDeposits();        break;
        case 5: account.listWithdrawals();     break;
        case 6: account.listAllTransactions(); break;
        case 7: std::cout << "\nExiting …\n";  break;
        default: std::cout << "Choose 1-7 only.\n";
        }
    }

    account.saveHistory();                    // Persist all changes

    // Display both starting and final balances for the required screenshot
    std::cout << std::fixed << std::setprecision(2)
        << "Starting balance: $" << startingBalance << '\n'
        << "Final balance:    $" << account.getBalance() << "\n";

    return 0;
}

