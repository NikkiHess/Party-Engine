import json
import random
import string

def generate_actor(is_player=False):
    actor = {}

    if not is_player:
        actor["name"] = ''.join(random.choice(string.ascii_letters) for _ in range(random.randint(3, 10)))
        actor["x"] = random.randint(-1000, 1000)
        actor["y"] = random.randint(-1000, 1000)
        actor["view"] = random.choice(string.ascii_uppercase)
        actor["vel_x"] = random.randint(-10, 10)
        actor["vel_y"] = random.randint(-10, 10)
        actor["nearby_dialogue"] = ''.join(random.choice(string.ascii_letters + string.punctuation + string.digits) for _ in range(random.randint(5, 20)))
        actor["contact_dialogue"] = ''.join(random.choice(string.ascii_letters + string.punctuation + string.digits) for _ in range(random.randint(5, 20)))
        actor["blocking"] = random.choice([True, False])
    else:
        actor["name"] = "player"
        actor["view"] = "p"
        actor["x"] = 0
        actor["y"] = 0

    return actor

def generate_test_scene(num_actors):
    player_added = False
    actors = []

    for _ in range(num_actors - 1):
        actor = generate_actor()
        actors.append(actor)

        # Ensure only one player is added
        if not player_added and actor["name"] == "player":
            player_added = True

    # Add the player if not added in the loop
    if not player_added:
        player = generate_actor(is_player=True)
        actors.append(player)

    data = {"actors": actors}
    return json.dumps(data, indent=2)

def save_to_file(data, filename):
    with open(filename, 'w') as file:
        file.write(data)

if __name__ == "__main__":
    num_actors = 1000000
    output_filename = "1m.scene"

    test_scene_data = generate_test_scene(num_actors)
    save_to_file(test_scene_data, output_filename)

    print(f"{num_actors} actors generated and saved to {output_filename}")
