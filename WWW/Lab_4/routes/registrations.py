from fastapi import APIRouter
from models.registration import Registration

router = APIRouter()

@router.get("/")
async def get_registrations():
    return await Registration.find_all().to_list()

@router.post("/")
async def create_registration(registration: Registration):
    await registration.insert()
    return {"msg": "User created", "id": str(registration.id)}
