# FPSDemo Source Structure

```
Source/
├── FPSDemo.Target.cs
├── FPSDemoEditor.Target.cs
│
└── FPSDemo/
    ├── FPSDemo.Build.cs
    │
    ├── Private/
    │   ├── AI/
    │   │   └── ZombieAIController.cpp
    │   │
    │   ├── Character/
    │   │   ├── FPSDemoCharacter.cpp
    │   │   ├── FPSDemoPlayerController.cpp
    │   │   └── Zombies/
    │   │       ├── HeavyZombie.cpp
    │   │       ├── LightZombie.cpp
    │   │       ├── RangedZombie.cpp
    │   │       └── ZombieBase.cpp
    │   │
    │   ├── Core/
    │   │   └── FPSDemo.cpp
    │   │
    │   ├── Framework/
    │   │   └── (empty)
    │   │
    │   ├── Game/
    │   │   └── FPSDemoGameMode.cpp
    │   │
    │   ├── Interaction/
    │   │   └── (empty)
    │   │
    │   ├── UI/
    │   │   └── (empty)
    │   │
    │   ├── Utils/
    │   │   └── (empty)
    │   │
    │   └── Weapon/
    │       ├── Components/
    │       │   ├── TP_PickUpComponent.cpp
    │       │   └── TP_WeaponComponent.cpp
    │       └── Projectiles/
    │           └── FPSDemoProjectile.cpp
    │
    └── Public/
        ├── AI/
        │   └── ZombieAIController.h
        │
        ├── Character/
        │   ├── FPSDemoCharacter.h
        │   ├── FPSDemoPlayerController.h
        │   └── Zombies/
        │       ├── HeavyZombie.h
        │       ├── LightZombie.h
        │       ├── RangedZombie.h
        │       └── ZombieBase.h
        │
        ├── Core/
        │   └── FPSDemo.h
        │
        ├── Framework/
        │   └── (empty)
        │
        ├── Game/
        │   └── FPSDemoGameMode.h
        │
        ├── Interaction/
        │   └── (empty)
        │
        ├── UI/
        │   └── (empty)
        │
        ├── Utils/
        │   └── (empty)
        │
        └── Weapon/
            ├── Components/
            │   ├── TP_PickUpComponent.h
            │   └── TP_WeaponComponent.h
            └── Projectiles/
                └── FPSDemoProjectile.h
```

## Module Overview

| Module | Description |
|--------|-------------|
| **Core** | Module definition and core types |
| **Character** | Player character, controller, and zombie classes |
| **AI** | AI controllers and behavior |
| **Weapon** | Weapon components and projectiles |
| **Game** | Game mode and rules |
| **Framework** | Framework classes (reserved) |
| **Interaction** | Interaction system (reserved) |
| **UI** | UI widgets and HUD (reserved) |
| **Utils** | Utility classes and helpers (reserved) |
