import jose
from fastapi import APIRouter, HTTPException, Depends
from passlib.context import CryptContext
from jose import jwt
from datetime import datetime, timedelta
from models.user import User, UserCreate, UserLogin
from dotenv import load_dotenv
import os

router = APIRouter()
load_dotenv()

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
SECRET_KEY = os.getenv("SECRET_KEY")
ALGORITHM = os.getenv("ALGORITHM")
ACCESS_TOKEN_EXPIRE_MINUTES = int(os.getenv("ACCESS_TOKEN_EXPIRE_MINUTES"))


def hash_password(password: str):
    return pwd_context.hash(password)

def verify_password(plain, hashed):
    return pwd_context.verify(plain, hashed)

def create_token(data: dict, expires_delta: timedelta):
    to_encode = data.copy()
    to_encode["exp"] = datetime.utcnow() + expires_delta
    return jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)


@router.post("/register")
async def register(user: UserCreate):
    if await User.find_one(User.email == user.email):
        raise HTTPException(status_code=400, detail="Email already registered")
    new_user = User(
        email=user.email,
        username=user.username,
        password_hash=hash_password(user.password)
    )
    await new_user.insert()
    return {"message": "User registered"}

@router.post("/login")
async def login(credentials: UserLogin):
    user = await User.find_one(User.email == credentials.email)
    if not user or not verify_password(credentials.password, user.password_hash):
        raise HTTPException(status_code=401, detail="Invalid credentials")
    token = create_token(
        {"sub": str(user.id), "role": user.role},
        timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    )
    return {"access_token": token, "token_type": "bearer"}
