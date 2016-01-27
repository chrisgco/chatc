# chatc
Chat client and server written in C

```
clang client.c 104.129.86.9314 - Connect to specific server

login or signup: login

username: iieqwuigfeiwq
password: hwqihiqhii

connected

failed to connect

conversations:
- @john john smith
- @kyle kyle roberts

who do you want to talk to (@username): @kyle

Message History with Kyle Roberts (@kyle):

Kyle (Dec, 5, 2015 at 10:01pm): hewifh ewhfoewqhfewh
You (Dec 5, 2015 at 10:02pm): hhefhew ehwifeho heiwoq

Compose: hey what's up

You (Dec 6, 2015 at 4:59pm): hey what's up

Compose:

Kyle (Dec 6, 2015 at 5:00pm): Hey

Compose:
```

## Server Client Commands
- `\SUCCESS`: Successful operation
- `\FAILURE`: Failed operation
- `\LOGIN\@username\password`: Login with `@username` and `password`
- `\SIGNUP\first name\last name\@username\password`: Sign up with `first name`, `last name`, `@username`, and `password`
- `\CONVERSATIONS`: get conversations for current user
- `\HISTORY\@username`: get conversation history for current user and user with `@username`
- `\COMPOSE\@username\message`: send `message` to `@username` ()
- `\MESSAGE\@username\date\message`: send `message` to current user from `@username` on `date` (from server to client)
