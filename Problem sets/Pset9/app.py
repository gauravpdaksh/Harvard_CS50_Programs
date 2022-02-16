import os
from string import ascii_letters, digits, punctuation
from datetime import date

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Custom filter to convert to usd format
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    # Getting information about the user
    UserId = session["user_id"]
    UserName = db.execute("SELECT username FROM users WHERE id = ?", UserId)

    # Getting the symbols and names of shares owned by the user
    shares = db.execute("SELECT symbol, company_name FROM shares WHERE id IN (SELECT share_id FROM transactions WHERE user_id = ? GROUP BY share_id) ORDER BY symbol", UserId)

    length = len(shares)

    # Getting the numbers of each share owned
    shareIds = db.execute("SELECT transactions.share_id AS sID, shares.symbol FROM transactions JOIN shares ON shares.id = transactions.share_id WHERE transactions.user_id = ? GROUP BY transactions.share_id ORDER BY shares.symbol", UserId)

    # Getting the quantity of each share owned by the user
    am = [None] * length

    for j in range(length):
        amount = db.execute("SELECT SUM(transactions.amount) AS Amount FROM transactions JOIN shares ON shares.id = transactions.share_id WHERE transactions.user_id = ? AND transactions.share_id = ?", UserId, shareIds[j]["sID"])

        am[j] = amount[0]["Amount"]

    # Preparing for viewing the portfolio
    portfolio = [None] * length

    # Value of portfolio
    portfolioValue = 0

    for i in range(length):
        a = shares[i]["symbol"]
        b = shares[i]["company_name"]
        c = am[i]

        # Getting current price of this share
        quote = lookup(a)

        if not quote:
            return apology("invalid symbol, or something else went wrong. try again", 400)

        d = quote["price"]
        e = (float(c) * float(d))

        portfolioValue += e

        portfolio[i] = {
            "symbol": a,
            "name": b,
            "amount": c,
            "price": d,
            "totalPrice": e
        }

    # Balance left
    user = db.execute("SELECT cash FROM users WHERE id = ?", UserId)
    balance = float(user[0]["cash"])

    # Total capital (Current balance + Portfolio's value)
    total = balance + portfolioValue

    return render_template("index.html", portfolio=portfolio, balance=balance, total=total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    # User reached route via POST
    if request.method == "POST":
        # Getting information about the user
        UserId = session["user_id"]
        UserName = db.execute("SELECT username FROM users WHERE id = ?", UserId)
        Username = UserName[0]["username"]

        # Getting the inputs from the user
        symbol = request.form.get("symbol")
        amount = request.form.get("shares")

        # Checking missing entries
        if not symbol:
            return apology("missing symbol", 400)
        elif not amount:
            return apology("missing amount", 400)

        if int(amount) < 1:
            return apology("value of 'Shares' must not be less than 1", 400)

        Symbol = request.form.get("symbol").upper()

        # Getting information about this share
        share = lookup(Symbol)

        # Validate symbol
        if not share:
            return apology("invalid symbol, or something else went wrong. try again", 400)

        # Getting current balance of user
        balance = db.execute("SELECT cash FROM users WHERE username = ?", Username)

        # Converting to float for doing calculations
        Price = float(share["price"])
        Amount = float(amount)
        Balance = float(balance[0]["cash"])

        # Checking if enough balance to buy
        if (Price * Amount) > Balance:
            return apology("can't afford", 400)

        # Filling the 'time' table in the database
        Time = share["time"]
        entryTime = db.execute("SELECT id FROM time WHERE exact_time= ?", Time)

        if len(entryTime) == 0:
            # Insert this time stamp into the 'time' table of the database
            db.execute("INSERT INTO time (exact_time) VALUES(?)", Time)

        EntryTime = db.execute("SELECT id FROM time WHERE exact_time= ?", Time)
        Time_id = EntryTime[0]["id"]

        # Filling up 'shares' table of the database
        entryShares = db.execute("SELECT id FROM shares WHERE symbol = ?", Symbol)

        if len(entryShares) == 0:
            # Insert this share into the shares table of database
            db.execute("INSERT INTO shares (symbol, company_name) VALUES(?, ?)", share["symbol"], share["name"])

        EntryShares = db.execute("SELECT id FROM shares WHERE symbol = ?", Symbol)

        # Updating the 'transactions' table in the database
        currentDate = date.today()
        ShareId = EntryShares[0]["id"]
        Clock = share["clock_time"]

        db.execute("INSERT INTO transactions (user_id, share_id, amount, price, date, clock_time, time_id) VALUES(?, ?, ?, ?, ?, ?, ?)", UserId, ShareId, Amount, Price, currentDate, Clock, Time_id)

        # Updating the balance
        totalPrice = (Price * Amount)
        Balance -= totalPrice
        db.execute("UPDATE users SET cash = ? WHERE username = ?", Balance, Username)

        # Display the updated portfolio on the index page (with a flash message)
        flash('Successfully bought!')
        return redirect("/")

    # User reached route via GET
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    # Getting information about the user
    UserId = session["user_id"]
    UserName = db.execute("SELECT username FROM users WHERE id = ?", UserId)
    Username = UserName[0]["username"]

    # Getting the symbols and names of shares owned by the user
    shares = db.execute("SELECT shares.symbol AS sym, shares.company_name AS com FROM shares JOIN transactions ON shares.id = transactions.share_id WHERE transactions.user_id = ? ORDER BY transactions.time_id DESC", UserId)

    # Getting the relevant entries from the 'transactions' table of the database
    allTrans = db.execute("SELECT amount, price, date, clock_time FROM transactions WHERE user_id = ? ORDER BY date, time_id DESC", UserId)

    # Getting the length of the 'transactions' table of the database
    Length = len(allTrans)

    # Preparing for viewing the history of transactions done by the user
    transactions = [None] * Length

    for l in range(Length):
        a = shares[l]["sym"]
        b = shares[l]["com"]
        c = allTrans[l]["amount"]
        d = allTrans[l]["price"]
        e = allTrans[l]["date"]
        f = allTrans[l]["clock_time"]

        transactions[l] = {
            "symbol": a,
            "name": b,
            "amount": c,
            "price": d,
            "date": e,
            "clock_time": f
        }

    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    # User reached route via POST
    if request.method == "POST":

        # Get the symbol from user
        Symbol = request.form.get("symbol").upper()

        if not Symbol:
            return apology("missing symbol", 400)

        # Get information about this share
        share = lookup(Symbol)

        if not share:
            return apology("invalid symbol, or something else went wrong. try again", 400)

        else:
            entryShares = db.execute("SELECT * FROM shares WHERE symbol = ?", share["symbol"])

            if len(entryShares) == 0:
                # Insert this share into the shares table of database
                db.execute("INSERT INTO shares (symbol, company_name) VALUES(?, ?)", share["symbol"], share["name"])

            # Display the quote on the page
            return render_template("quoted.html", companyName=share["name"], symbol=share["symbol"], price=share["price"])

    # User reached route via GET
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    # User reached route via POST
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Preparing to check whether the password is according to the guidelines provided to the user on the page
        Password = request.form.get("password")
        numberCount = 0
        letterCount = 0
        symbolCount = 0

        if len(Password) < 6:
            return apology("password must be atleast 6 characters long", 400)

        # Iterating over each character of the password
        for element in Password:
            if element in digits:
                numberCount += 1
            elif element in ascii_letters:
                letterCount += 1
            elif element in punctuation:
                symbolCount += 1
            else:
                return apology("you used wierd symbol(s) in the password", 400)

        # If no letter used
        if letterCount == 0:
            return apology("password must contain atleast one letter", 400)
        # If no digit used
        if numberCount == 0:
            return apology("password must contain atleast one digit", 400)
        # If no symbol used
        if symbolCount == 0:
            return apology("password must contain atleast one symbol", 400)

        # Ensure password was submitted again for confirmation
        elif not request.form.get("confirmation"):
            return apology("must provide password again for confirmation", 400)

        # Check if passwords match
        if request.form.get("password") != request.form.get("confirmation"):
            return apology("passwords don't match", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username doesn't exists already
        if len(rows) != 0:
            return apology("this username is not available, please choose another one", 400)

        # Saving the username and the hash of the password provided by the user
        hashed = generate_password_hash(request.form.get("password"))
        Username = request.form.get("username")
        db.execute("INSERT INTO users (username, hash) VALUES(?, ?)", Username, hashed)

        # Log-in the user and remember which user has logged in
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page (with a flash message)
        flash('You were successfully registered!')
        return redirect("/")

    # User reached route via GET
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    # Getting information about the user
    UserId = session["user_id"]
    UserName = db.execute("SELECT username FROM users WHERE id = ?", UserId)
    Username = UserName[0]["username"]

    # User reached route via POST
    if request.method == "POST":

        # Getting the inputs from the user
        symbol = request.form.get("symbol")
        amount = request.form.get("shares")

        # Checking missing entries
        if not symbol:
            return apology("missing symbol", 400)
        elif not amount:
            return apology("missing amount", 400)

        if int(amount) < 1:
            return apology("value of 'Shares' must not be less than 1", 400)

        Symbol = request.form.get("symbol").upper()

        # Getting all the symbols of all the shares owned by the user
        shares = db.execute("SELECT symbol FROM shares WHERE id IN (SELECT share_id FROM transactions WHERE user_id = ? GROUP BY share_id) ORDER BY symbol", UserId)

        length = len(shares)

        # Validating the symbol providing by the user to protect against malicious attempts
        sharesOwned = [None] * length

        for k in range(length):
            sharesOwned[k] = shares[k]["symbol"]

        if Symbol not in sharesOwned:
            return apology("hacking attempt", 400)

        # Validating the sale
        shareId = db.execute("SELECT id FROM shares WHERE symbol = ?", Symbol)

        Amount = db.execute("SELECT SUM(transactions.amount) AS Amount FROM transactions JOIN shares ON shares.id = transactions.share_id WHERE transactions.user_id = ? AND transactions.share_id = ?", UserId, shareId[0]["id"])

        if int(amount) > int(Amount[0]["Amount"]):                # If trying to sell more shares than he/she owns
            return apology("you don't have that many shares of this company", 400)

        # Get information about this share
        share = lookup(Symbol)

        if not share:
            return apology("invalid symbol, or something else went wrong. try again", 400)

        # Getting current balance of user
        balance = db.execute("SELECT cash FROM users WHERE username = ?", Username)

        Price = float(share["price"])
        Amount = 0.0 - float(amount)
        Balance = float(balance[0]["cash"])

        # Updating the 'transactions' table in the database
        Time = share["time"]
        entryTime = db.execute("SELECT id FROM time WHERE exact_time= ?", Time)

        if len(entryTime) == 0:
            # Insert this time stamp into the 'time' table of the database
            db.execute("INSERT INTO time (exact_time) VALUES(?)", Time)

        EntryTime = db.execute("SELECT id FROM time WHERE exact_time= ?", Time)
        Time_id = EntryTime[0]["id"]

        EntryShares = db.execute("SELECT id FROM shares WHERE symbol = ?", Symbol)
        ShareId = EntryShares[0]["id"]

        currentDate = date.today()
        Clock = share["clock_time"]

        db.execute("INSERT INTO transactions (user_id, share_id, amount, price, date, clock_time, time_id) VALUES(?, ?, ?, ?, ?, ?, ?)", UserId, ShareId, Amount, Price, currentDate, Clock, Time_id)

        # Updating the balance
        totalPrice = (Price * float(amount))
        Balance += totalPrice
        db.execute("UPDATE users SET cash = ? WHERE username = ?", Balance, Username)

        # Display the updated portfolio on the index page (with a flash message)
        flash('Sold!')
        return redirect("/")

    # User reached route via GET
    else:
        # Preparing to get the symbols of the shares owned by the user to display them on the 'sell' page
        # Getting the symbols and names of shares owned by the user
        shares = db.execute("SELECT symbol, company_name FROM shares WHERE id IN (SELECT share_id FROM transactions WHERE user_id = ? GROUP BY share_id) ORDER BY symbol", UserId)

        length = len(shares)

        # Getting the numbers of each share owned
        shareIds = db.execute("SELECT transactions.share_id AS sID, shares.symbol FROM transactions JOIN shares ON shares.id = transactions.share_id WHERE transactions.user_id = ? GROUP BY transactions.share_id ORDER BY shares.symbol", UserId)

        # Getting the quantity of each share owned by the user
        am = [None] * length

        for i in range(length):
            amount = db.execute("SELECT SUM(transactions.amount) AS Amount FROM transactions JOIN shares ON shares.id = transactions.share_id WHERE transactions.user_id = ? AND transactions.share_id = ?", UserId, shareIds[i]["sID"])

            am[i] = amount[0]["Amount"]

        sharesOwned = [None] * length

        for j in range(length):
            a = shares[j]["symbol"]
            b = am[j]

            sharesOwned[j] = {
                "symbol": a,
                "amount": b
            }

        return render_template("sell.html", sharesOwned=sharesOwned)