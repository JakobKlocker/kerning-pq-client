# Party Quest Completer

## Introduction
The goal of this project was to **automatically complete the Kerning Party Quest**, which is a party quest in the nostalgic online MMORPG called **Maplestory**. The Party Quest has five stages and requires at least three players to complete. 

**At the end of the readme you'll find a video of this project in action**.

---

## Architecture
The architecture of this project is complex, since we had¹ to inject a DLL into each game process. We chose to have each game process listen on a unique TCP port for actions and share Data over Named Shared Memory. Above these individual clients, a managing tool was used to oversee and coordinate the logic of the Party Quest.

In this write-up:
- The **Managing tool** is referred to as the **Server**.
- The **Injected code** in each game process is referred to as the **Client**.

---

## Client
Each client was injected into the game process to enable or disable hacks and log necessary packets. Since the Server needed specific information to determine actions, we shared all essential variables with the Server every second via Named Shared Memory².

### Commands
The client was programmed to execute the following commands:
- **sendpacket** [Packet] - *Sends a packet to the Game Server*
- **teleport** [X][Y] - *Teleporst the character to a certain X/Y*
- **rope** ["True"]/["False"] - *Toggles Rope usage on/off*
- **attack** ["True"]/["False"] - *Toggles Attack on/off*
- **loot** ["True"]/["False"] - *Toggles Loot on/off*
- **portal** - *Uses the portal*
- **getcount** [ItemID] - *calls a function to get the count of a specific item ID*

### Hacks
To gather information on specific game functions, we started by coding a Packet Editor. With the returned functions, we were able to implement the following hacks in the client:

- **Always On Hacks**:
  - Aircheck
  - Log Packets
  - Auto Pot

- **Triggered Hacks/Functions**:
  - Use Rope
  - Auto Attack
  - Auto Loot
  - Teleport to specific (X, Y) coordinates
  - Send Packet

---

## Shared Data
To enable the Server to complete the Party Quest, we hooked into functions to provide the following data points:
- Character Position (X, Y)
- Character HP and MP
- Mob Position (X, Y)
- Mob Count
- Item Position (X, Y)
- Item Count
- Map ID
- Current Tickets and Tickets Needed
- Item Information
- Party Leader Status
- Stage Cleared Status
- Login Screen Status
- Channel Selected
- Character Selected

---

## Server
Since this is a Proof of Concept, the Server implementation is only described briefly. The **Server** handled opening three game processes, injecting DLLs, managing the Party Quest logic via provided APIs, and restarting clients upon disconnection or crashes.

---

## Challenges
The challenges encountered in this project can be broken down into three main areas:

### 1. Reversing
Reversing the game was one of the most challenging yet enjoyable aspects. We began by coding a Packet Editor to identify the return address of the send function for locating specific functions. Since the send function was encrypted, reversing took more effort than anticipated. Once decrypted, locating the necessary hacks and data points became much easier.

### 2. Logic
The Party Quest logic had to be fully managed by the Server. Certain stages required reading messages from NPC chat to determine the number of tickets each character needed during the first stage. This involved logging and interpreting received packets, which added complexity to the task.

### 3. Coding
The coding phase was relatively quick, though the code remains somewhat messy due to its Proof of Concept nature. Game hacking involves altering assembly functions and calling existing game functions, which I always enjoy doing. A new aspect I learned was using named shared memory in Windows, which was easier to implement than expected.

## Watch the Video


https://github.com/user-attachments/assets/a797c05c-c94d-4a0e-b236-9cccca54c3b5




---

¹ - We wanted to, it's a lot easier and more efficent to have it running internaly  
² - **Named Shared Memory**: [Creating Named Shared Memory in Windows](https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory)
