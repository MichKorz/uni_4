from fastapi import FastAPI
from db import init_db
from routes import users, events, registrations, auth

app = FastAPI()

@app.on_event("startup")
async def startup_event():
    await init_db()

app.include_router(users.router, prefix="/users", tags=["Users"])
app.include_router(events.router, prefix="/events", tags=["Events"])
app.include_router(registrations.router, prefix="/registrations", tags=["Registrations"])
app.include_router(auth.router, prefix="/auth", tags=["auth"])
