from beanie import Document, PydanticObjectId
from pydantic import EmailStr, BaseModel


class Registration(Document):
    user_id: PydanticObjectId
    event_id: PydanticObjectId
    status: str  # e.g. "registered", "cancelled"

    class Settings:
        name = "registrations"




