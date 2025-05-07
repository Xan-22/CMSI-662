"""
This module provides secure and efficient user authentication services for the banking application. 
It includes functionalities such as:
- User login and authentication.
- Token generation and validation using JSON Web Tokens (JWT).
- Password hashing and verification using the `pbkdf2_sha256` algorithm.

Constants:
    - MIN_PASSWORD_LENGTH (int): Minimum allowed length for user passwords.
    - MAX_PASSWORD_LENGTH (int): Maximum allowed length for user passwords.
    - MAX_RATE_LIMIT (int): Maximum number of allowed login attempts.
    - SECRET (str): Secret key for encoding and decoding JWT tokens. 
      This key is loaded from environment variables.

Functions:
    - login_required(func): A decorator to enforce user authentication for protected routes.
    - get_user_with_credentials(email, password): Authenticates a user by their email and password.
    - logged_in(): Checks if the user is logged in by validating the authentication token.
    - create_token(email): Generates a JWT for the given email.

Notes:
    - The `login_required` decorator ensures that only authenticated users 
      can access certain routes.
    - Passwords are securely hashed and verified using `pbkdf2_sha256`.
    - Artificial delays are introduced in authentication to mitigate timing attacks.
    - JWT tokens include claims for subject (`sub`), issued at (`iat`), and expiration (`exp`).
"""

from functools import wraps
import os
import sqlite3
import datetime
import time
from typing import Final

from passlib.hash import pbkdf2_sha256
from flask import render_template, request, g
from dotenv import load_dotenv
import jwt

MIN_PASSWORD_LENGTH: Final[int] = 6
MAX_PASSWORD_LENGTH: Final[int] = 999
MAX_RATE_LIMIT: Final[int] = 5

load_dotenv()
SECRET = os.getenv('SECRET')
if not SECRET:
    raise ValueError(
        "The SECRET environment variable is not set. Please set it in your environment.")


# Decorator to check if user is logged in. Used to protect routes that require authentication.
def login_required(func):
    """
    A decorator to enforce user authentication before accessing a view function.

    This decorator checks if the user is logged in by calling the `logged_in()` function.
    If the user is not logged in, it renders the the login page (the "login.html" template).
    If the user is logged in, it allows the wrapped function to execute as normal.

    Args:
        func (Callable): The view function to be wrapped by the decorator.

    Returns:
        Callable: The wrapped function that enforces the login requirement.
    """
    @wraps(func)
    def decorated_function(*args, **kwargs):
        if not logged_in():
            return render_template("login.html")
        return func(*args, **kwargs)
    return decorated_function


def get_user_with_credentials(email, password):
    """
    Authenticate a user by their email and password.

    This function checks if the provided email and password match a user
    in the database. If the credentials are valid, it returns a dictionary
    containing the user's email, name, and a generated authentication token.
    Otherwise, it returns None.

    Args:
        email (str): The email address of the user.
        password (str): The plaintext password of the user.

    Returns:
        dict or None: A dictionary with the user's email, name, and token if
        authentication is successful, or None if the credentials are invalid.

    Raises:
        sqlite3.Error: If there is an issue connecting to or querying the database.

    Notes:
        - The function enforces a minimum and maximum password length using
          the constants MIN_PASSWORD_LENGTH and MAX_PASSWORD_LENGTH.
        - Passwords are verified using the `pbkdf2_sha256` hashing algorithm.
        - An artificial delay is introduced to mitigate timing attacks.
        - The database connection is always closed after the operation.
    """
    con = None
    try:
        if len(password) < MIN_PASSWORD_LENGTH or len(password) > MAX_PASSWORD_LENGTH:
            return None
        con = sqlite3.connect('bank.db')
        cur = con.cursor()
        cur.execute('''
            SELECT email, name, password FROM users where email=?''',
                    (email,))
        row = cur.fetchone()
        if row is None:
            return None
        email, name, hashed_password = row
        if not pbkdf2_sha256.verify(password, hashed_password):
            return None
        return {"email": email, "name": name, "token": create_token(email)}
    finally:
        time.sleep(1)  # Artificial delay to prevent timing attacks
        if con:
            con.close()


def logged_in():
    """
    Checks if the user is logged in by validating the authentication token 
    stored in the request cookies.

    Returns:
        bool: True if the user is logged in and the token is valid, False otherwise.

    Side Effects:
        - Sets `g.user` to the user identifier (`sub`) from the decoded token 
          if the token is valid.

    Exceptions:
        - Catches `jwt.InvalidTokenError` if the token is invalid or cannot be decoded.
    """
    token = request.cookies.get('auth_token')
    try:
        if token is None:
            return False
        data = jwt.decode(token, SECRET, algorithms=['HS256'])
        g.user = data['sub']
        return True
    except jwt.InvalidTokenError:
        return False


def create_token(email):
    """
    Generates a JSON Web Token (JWT) for the given email.

    Args:
        email (str): The email address to include in the token's payload.

    Returns:
        str: A JWT string encoded with the specified payload, secret, and algorithm.

    The token includes the following claims:
        - 'sub': Subject of the token, set to the provided email.
        - 'iat': Issued at timestamp, set to the current UTC time.
        - 'exp': Expiration timestamp, set to 60 minutes from the issued time.
    """
    now = datetime.datetime.now(datetime.timezone.utc)
    payload = {'sub': email, 'iat': now,
               'exp': now + datetime.timedelta(minutes=60)}
    token = jwt.encode(payload, SECRET, algorithm='HS256')
    return token
