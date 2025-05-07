"""
This module implements a Flask web application for a simple banking system. 
It includes functionality for user authentication, account management, 
and money transfers. The application uses CSRF protection, rate limiting, 
and login-required decorators to enhance security.
Routes:
- `/`: Home page that redirects to the login page if the user is not logged in.
- `/login`: Handles user login with rate limiting to prevent brute-force attacks.
- `/dashboard`: Displays the user's dashboard, showing account details.
- `/details`: Displays account details for a specific account.
- `/transfer`: Handles money transfers between accounts with validation.
- `/logout`: Logs the user out by deleting the authentication token.
Security Features:
- CSRF protection using Flask-WTF.
- Rate limiting using Flask-Limiter to prevent brute-force and DoS attacks.
- Login-required decorators to restrict access to authenticated users only.
Dependencies:
- Flask: Web framework for building the application.
- Flask-WTF: Provides CSRF protection.
- Flask-Limiter: Implements rate limiting.
- `user_service`: Custom module for user authentication and session management.
- `account_service`: Custom module for account balance and transfer operations.
"""

import os
from typing import Final
from flask import Flask, flash, request, make_response, redirect, render_template, g, abort
from flask_wtf.csrf import CSRFProtect
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address
from user_service import get_user_with_credentials, logged_in, login_required
from account_service import get_balance, do_transfer, validate_transfer, get_account_numbers


app = Flask(__name__)
LOGIN_PAGE: Final[str] = "login.html"
INVALID_CREDENTIALS_MSG: Final[str] = "Invalid credentials"

app.config['SECRET_KEY'] = os.getenv('SECRET')
csrf = CSRFProtect()  # Using Flasks built-in CSRF protection
csrf.init_app(app)

limiter = Limiter(  # Rate limiter to prevent brute-force and DoS attacks.
    get_remote_address,
    app=app,
    # All requests are rate limited to 5 per minute by default.
    default_limits=["5 per minute"],
    storage_uri="memory://",
)


@app.route("/", methods=['GET'])
def home():
    """
    Handles the logic for the home route.

    If the user is not logged in, it renders the login page.
    If the user is logged in, it redirects them to the dashboard.

    Returns:
        Response: A rendered template for the login page if the user is not logged in.
        Redirect: A redirect to the dashboard if the user is logged in.
    """
    if not logged_in():
        return render_template(LOGIN_PAGE)
    return redirect('/dashboard')


@app.route("/login", methods=["POST"])
# Rate limit the login route to 5 requests per five minutes to slow password guessing attempts.
@limiter.limit("5 per 5 minutes")
def login():
    """
    Handles the login functionality for the application.
    This function retrieves the email and password from the request form,
    validates the input, and attempts to authenticate the user. If the 
    credentials are valid, the user is redirected to the dashboard with 
    an authentication token set as a cookie. Otherwise, an error message 
    is displayed on the login page.
    Returns:
        Response: A redirect response to the dashboard with a 303 status 
        code if authentication is successful.
        TemplateResponse: A rendered login page with an error message if 
        authentication fails or input is invalid.
    """
    email = request.form.get("email")
    password = request.form.get("password")
    if email is None or password is None:
        return render_template(LOGIN_PAGE, error="Email and password are required")

    user = get_user_with_credentials(email, password)
    if not user:
        return render_template(LOGIN_PAGE, error=INVALID_CREDENTIALS_MSG)
    response = make_response(redirect("/dashboard"))
    response.set_cookie("auth_token", user["token"])

    return response, 303


@app.route("/dashboard", methods=['GET'])
# Rate limit the dashboard route to 15 requests per minute, as it is a common endpoint.
@limiter.limit("15 per minute")
@login_required
def dashboard():
    """
    Render the dashboard page for a logged-in user.

    If the user is not logged in, they are redirected to the login page.
    Otherwise, the dashboard page is rendered with the user's email.

    Returns:
        Response: A redirect response to the login page if the user is not logged in,
                  or a rendered template of the dashboard page if the user is logged in.
    """
    if not logged_in():
        return make_response(redirect(LOGIN_PAGE))
    return render_template("dashboard.html", email=g.user)


@app.route("/accounts", methods=['GET'])
@login_required
def accounts():
    """
    Render the accounts page for a logged-in user.

    If the user is not logged in, redirects to the login page. Otherwise,
    retrieves the list of accounts associated with the user and displays
    them on the accounts page.

    Returns:
        Response: A rendered HTML template for the accounts page or
        the login page if the user is not authenticated.
    """
    if not logged_in():
        return render_template(LOGIN_PAGE)
    account_numbers = get_account_numbers(g.user)
    if account_numbers is None:
        return render_template("accounts.html", user=g.user, error="No accounts found")
    return render_template("accounts.html", user=g.user, account_numbers=account_numbers)


@app.route("/details", methods=['GET'])
@login_required
def details():
    """
    Render the account details page for a logged-in user.

    If the user is not logged in, redirects to the login page. Otherwise,
    retrieves the account number from the request arguments and displays
    the account details, including the balance.

    Returns:
        Response: A rendered HTML template for the account details page or
        the login page if the user is not authenticated.
    """
    if not logged_in():
        return render_template(LOGIN_PAGE)
    account_number = request.args['account']
    return render_template(
        "details.html",
        user=g.user,
        account_number=account_number,
        balance=get_balance(account_number, g.user))


@app.route("/transfer", methods=["GET", "POST"])
@login_required
def transfer():
    """
    Handles the transfer of funds between accounts.
    This function supports both GET and POST HTTP methods:
    - For GET requests, it renders the transfer page.
    - For POST requests, it processes the transfer of funds.
    Returns:
        - On GET: Renders the transfer page with the current user's information.
        - On successful POST: Redirects to the dashboard with a success message.
        - On failure: Returns an appropriate HTTP error response.
    Raises:
        - 400 Bad Request: If the amount is missing, invalid, or exceeds the available balance.
        - 404 Not Found: If the source account is not found.
    Workflow:
        1. Checks if the user is logged in. If not, redirects to the login page.
        2. For GET requests, renders the transfer form.
        3. For POST requests:
            - Validates the input fields (source, target, amount).
            - Ensures the transfer is valid and the source account has sufficient balance.
            - Performs the transfer and redirects to the dashboard on success.
            - Returns an error response if the transfer fails.
    Note:
        - The function uses helper functions such as `logged_in`, `validate_transfer`, 
          `get_balance`, and `do_transfer` for various operations.
        - Flash messages are used to notify the user of successful transfers.
    """
    if not logged_in():
        return render_template(LOGIN_PAGE)

    if request.method == "GET":
        return render_template("transfer.html", user=g.user)

    source = request.form.get("from")
    target = request.form.get("to")
    amount_str = request.form.get("amount")

    if amount_str is None:
        abort(400, "Amount is required and must be a number")

    try:
        amount = int(amount_str)
    except ValueError:
        abort(400, "Amount must be a number")

    transfer_validation_result = validate_transfer(source, target, amount)
    if transfer_validation_result[0] is False:
        abort(400, transfer_validation_result[1])

    available_balance = get_balance(source, g.user)
    if available_balance is None:
        abort(404, "Account not found")
    if amount > available_balance:
        abort(400, "You don't have that much")

    if do_transfer(source, target, amount):
        flash(f"Transfer of {amount} from {source} to {target} was successful")
        return redirect("/dashboard", 303)
    abort(400, "Something bad happened")
    # While the above line is not a good error message,
    # it is not a security risk to show it to the user.


@app.route("/logout", methods=['GET'])
def logout():
    """
    Logs the user out by deleting the 'auth_token' cookie and redirecting to the dashboard.

    This function creates an HTTP response that redirects the user to the "/dashboard" 
    route. It also deletes the 'auth_token' cookie from the user's browser to effectively 
    log them out. The response is returned with a 303 See Other status code.

    Returns:
        tuple: A tuple containing the HTTP response object and the HTTP status code (303).
    """
    response = make_response(redirect("/dashboard"))
    response.delete_cookie('auth_token')
    return response, 303
