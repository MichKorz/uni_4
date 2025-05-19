from beanie import Document, PydanticObjectId
from datetime import datetime
from pydantic import BaseModel

class Event(Document):
    title: str
    description: str
    date: datetime
    location: str
    category: str
    host: PydanticObjectId

    class Settings:
        name = "events"

class EventCreate(BaseModel):
    title: str
    description: str
    date: datetime
    location: str
    category: str