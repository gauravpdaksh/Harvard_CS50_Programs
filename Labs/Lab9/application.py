import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///birthdays.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET", "POST"])
def index():

    # For POST
    if request.method == "POST":

        # Getting the values
        name = request.form.get("name")
        month = request.form.get("month")
        day = request.form.get("day")

        # Getting all the entries from the database
        birthdays = db.execute("SELECT * FROM birthdays ORDER BY month, day")

        # Checking whether all entries have been provided
        if not name:
            return render_template("index.html", message="Error: Missing name.", birthdays=birthdays)
        if not month:
            return render_template("index.html", message="Error: Please provide the month of the birthday.", birthdays=birthdays)
        if not day:
            return render_template("index.html", message="Error: Please provide the day of the birthday.", birthdays=birthdays)


        # Validate day
        # For months with 31 days
        if int(month) in [1, 3, 5, 7, 8, 10, 12] and int(day) > 31:
            message = "Error: Please provide a valid date."
        # For months with 30 days
        elif int(month) in [4, 6, 9, 11] and int(day) > 30:
            message = "Error: Please provide a valid date."
        # Checking for February
        elif int(month) == 2 and int(day) > 29:
            message = "Error: Please provide a valid date."
        # If there is no error message to display
        else:
            # Add the user's entry into the database
            db.execute("INSERT INTO birthdays (name, month, day) VALUES(?, ?, ?)", name, month, day)

            return redirect("/")

        # If there is an error message to be displayed
        return render_template("index.html", message=message, birthdays=birthdays)


    # For GET
    else:

        # Display the entries in the database on index.html
        birthdays = db.execute("SELECT * FROM birthdays ORDER BY month, day")

        return render_template("index.html", birthdays=birthdays)
