from flask import Flask, request, make_response, redirect, render_template, g, abort
from flask_wtf.csrf import CSRFProtect

from user_service import get_user_with_credentials, logged_in
from account_service import get_balance, do_transfer
from typing import Final

app = Flask(__name__)
MAX_TRANSFER_AMOUNT: Final[int] = 1000
LOGIN_PAGE: Final[str] = "login.html"

app.config['SECRET_KEY'] = 'yoursupersecrettokenhere'
csrf = CSRFProtect(app) 

@app.route("/", methods=['GET'])
def home():
    if not logged_in():
        return render_template(LOGIN_PAGE)
    return redirect('/dashboard')


@app.route("/login", methods=["POST"])
def login():
    email = request.form.get("email")
    password = request.form.get("password")
    user = get_user_with_credentials(email, password)
    if not user:
        return render_template(LOGIN_PAGE, error="Invalid credentials")
    response = make_response(redirect("/dashboard"))
    response.set_cookie("auth_token", user["token"])
    print(f"User {user['email']} logged in")
    return response, 303


@app.route("/dashboard", methods=['GET'])
def dashboard():
    if not logged_in():
        return render_template(LOGIN_PAGE)
    return render_template("dashboard.html", email=g.user)


@app.route("/details", methods=['GET'])
def details():
    if not logged_in():
        return render_template(LOGIN_PAGE)
    account_number = request.args['account']
    return render_template(
        "details.html", 
        user=g.user,
        account_number=account_number,
        balance = get_balance(account_number, g.user))


@app.route("/transfer", methods=["GET", "POST"])
def transfer():
    if not logged_in():
        return render_template(LOGIN_PAGE)
    
    if request.method == "GET":
        return render_template("transfer.html", user=g.user)

    source = request.form.get("from")
    target = request.form.get("to")
    amount_str = request.form.get("amount")

    try:
        amount = int(amount_str)
    except ValueError:
        abort(400, "Amount must be a number")

    if amount < 0:
        abort(400, "Amount cannot be negative")
    if amount > MAX_TRANSFER_AMOUNT:
        abort(400, "Amount cannot exceed {MAX_TRANSFER_AMOUNT}")
    if source == target:
        abort(400, "You cannot transfer to the same account")
    if source == "" or target == "":
        abort(400, "You must enter a valid account number")

    available_balance = get_balance(source, g.user)
    if available_balance is None:
        abort(404, "Account not found")
    if amount > available_balance:
        abort(400, "You don't have that much")

    if do_transfer(source, target, amount):
        pass  # TODO GIVE FEEDBACK
    else:
        abort(400, "Something bad happened")

    response = make_response(redirect("/dashboard"))
    return response, 303


@app.route("/logout", methods=['GET'])
def logout():
    response = make_response(redirect("/dashboard"))
    response.delete_cookie('auth_token')
    return response, 303
