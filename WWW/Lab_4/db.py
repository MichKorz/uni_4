from beanie import init_beanie
from motor.motor_asyncio import AsyncIOMotorClient
from models.user import User
from models.event import Event
from models.registration import Registration
import os
from dotenv import load_dotenv

load_dotenv()

async def init_db():
    client = AsyncIOMotorClient(os.getenv("MONGO_URI"))
    await init_beanie(database=client.get_default_database(),
                      document_models=[User, Event, Registration])
