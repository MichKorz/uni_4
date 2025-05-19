from fastapi import APIRouter, Depends
from models.user import User
from routes.auth import require_admin, get_current_user
from pydantic import EmailStr

router = APIRouter()

@router.get("/")
async def get_all_users(admin_user: User = Depends(require_admin)):
    users = await User.find_all().to_list()
    return users

@router.patch("/")
async def update_email(new_email: EmailStr, user: User = Depends(get_current_user)):
    user.email = new_email
    await user.save()