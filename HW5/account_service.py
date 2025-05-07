"""
This module provides services for managing bank account operations such as 
retrieving account balances and performing account transfers.
Functions:
    get_balance(account_number: str, owner: str) -> Optional[float]:
        Retrieves the balance of a specific account for a given owner.
    do_transfer(source: str, target: str, amount: float) -> bool:
        Executes a transfer of a specified amount from a source account 
        to a target account.
    validate_transfer(source: str, target: str, amount: float) -> Tuple[bool, Optional[str]]:
        Validates the parameters for a transfer operation, ensuring that 
        the source and target accounts are valid and the transfer amount 
        adheres to defined constraints.
    get_account_numbers(owner: str) -> Optional[List[int]]:
        Retrieves all account numbers owned by a specific owner.
Constants:
    MAX_TRANSFER_AMOUNT (int): The maximum allowable amount for a single transfer.
"""

import sqlite3
from typing import Final

MAX_TRANSFER_AMOUNT: Final[int] = 1000


def get_balance(account_number, owner):
    """
    Retrieves the balance of a specific account from the database.

    Args:
        account_number (int): The unique identifier of the account.
        owner (str): The name of the account owner.

    Returns:
        float or None: The balance of the account if found, or None if the account does not exist.
    """
    con = None
    try:
        con = sqlite3.connect('bank.db')
        cur = con.cursor()
        cur.execute('''
            SELECT balance FROM accounts where id=? and owner=?''',
                    (account_number, owner))
        row = cur.fetchone()
        if row is None:
            return None
        return row[0]
    finally:
        if con:
            con.close()


def do_transfer(source, target, amount):
    """
    Transfers a specified amount of money from one account to another.

    Args:
        source (int): The ID of the source account from which the amount will be deducted.
        target (int): The ID of the target account to which the amount will be credited.
        amount (float): The amount of money to transfer.

    Returns:
        bool: True if the transfer was successful, False if the target account does not exist.

    Raises:
        sqlite3.DatabaseError: If there is an issue with the database connection or operations.

    Notes:
        - This function assumes that the source account has sufficient balance for the transfer.
        - The database file is assumed to be named 'bank.db'.
        - The function ensures that the database connection is properly closed after execution.
    """
    con = None
    try:
        con = sqlite3.connect('bank.db')
        cur = con.cursor()
        cur.execute('''
            SELECT id FROM accounts where id=?''',
                    (target,))
        row = cur.fetchone()
        if row is None:
            return False
        cur.execute('''
            UPDATE accounts SET balance=balance-? where id=?''',
                    (amount, source))
        cur.execute('''
            UPDATE accounts SET balance=balance+? where id=?''',
                    (amount, target))
        con.commit()
        return True
    finally:
        if con:
            con.close()


def validate_transfer(source, target, amount):
    """
    Validates the parameters for a transfer operation between accounts.

    Args:
        source (str): The account number of the source account.
        target (str): The account number of the target account.
        amount (float): The amount to be transferred.

    Returns:
        tuple: A tuple containing:
            - bool: True if the transfer is valid, False otherwise.
            - str or None: An error message if the transfer is invalid, or None if valid.

    Validation Rules:
        - The source and target accounts must not be the same.
        - Both source and target account numbers must be non-empty.
        - The transfer amount must be greater than zero.
        - The transfer amount must not exceed `MAX_TRANSFER_AMOUNT`.
    """
    if source == target:
        return False, "You cannot transfer to the same account"
    if source == "" or target == "":
        return False, "You must enter a valid account number"
    if amount <= 0:
        return False, "Amount cannot be zero or negative"
    if amount > MAX_TRANSFER_AMOUNT:
        return False, f"Amount cannot exceed {MAX_TRANSFER_AMOUNT}"
    return True, None


def get_account_numbers(owner):
    """
    Retrieves all owned account numbers for a given owner from the database.

    Args:
        account_number (int): The unique identifier of the account.
        owner (str): The name of the account owner.

    Returns:
        list[int] or None: A list of account numbers if found, or None if the owner has no accounts.
    """
    con = None
    try:
        con = sqlite3.connect('bank.db')
        cur = con.cursor()
        cur.execute('''
            SELECT id FROM accounts WHERE owner=?''',
                    (owner,))
        rows = cur.fetchall()
        if not rows:
            return None
        return [row[0] for row in rows]
    finally:
        if con:
            con.close()
