from beanie import Document
from pydantic import EmailStr
from datetime import datetime
from typing import Literal

class User(Document):
    username: str
    email: EmailStr
    password_hash: str
    role: Literal["user", "admin"] = "user"
    created_at: datetime = datetime.utcnow()

    class Settings:
        name = "users"
