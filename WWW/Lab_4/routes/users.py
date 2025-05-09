from fastapi import APIRouter
from models.user import User

router = APIRouter()

@router.get("/")
async def get_users():
    return await User.find_all().to_list()

@router.post("/")
async def create_user(user: User):
    await user.insert()
    return {"msg": "User created", "id": str(user.id)}
