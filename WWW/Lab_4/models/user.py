from beanie import Document
from pydantic import EmailStr, BaseModel
from datetime import datetime
from typing import Literal

class User(Document):
    username: str
    email: EmailStr
    password_hash: str
    role: Literal["user", "admin"]
    created_at: datetime = datetime.utcnow()

    class Settings:
        name = "users"

class UserCreate(BaseModel):
    email: EmailStr
    username: str
    password: str
    role: Literal["user", "admin"]

class UserLogin(BaseModel):
    email: EmailStr
    password: str
