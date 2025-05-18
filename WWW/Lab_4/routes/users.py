from fastapi import APIRouter, Depends
from models.user import User
from routes.auth import require_admin

router = APIRouter()

@router.get("/")
async def get_all_users(admin_user: User = Depends(require_admin)):
    users = await User.find_all().to_list()
    return users

@router.post("/")
async def create_user(user: User):
    await user.insert()
    return {"msg": "User created", "id": str(user.id)}
