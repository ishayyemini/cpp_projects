import argparse
import os
import re
import time

import pygame

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GRAY = (100, 100, 100)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)
YELLOW = (255, 255, 0)
BROWN = (139, 69, 19)
GOLD = (255, 215, 0)
SILVER = (192, 192, 192)
DARK_OVERLAY = (0, 0, 0, 180)  # Semi-transparent black


# Game object types
class ObjectType:
    TANK = 'T'
    SHELL = '*'
    MINE = '@'
    WALL = '#'


class TankVisualizer:
    def __init__(self, game_state_file, cell_size=40, delay=0.2, debug=False):
        pygame.init()
        self.cell_size = cell_size
        self.delay = delay
        self.game_state_file = game_state_file
        self.width = 0
        self.height = 0
        self.screen = None
        self.font = pygame.font.SysFont('Arial', 16)
        self.title_font = pygame.font.SysFont('Arial', 24)
        self.game_over_font = pygame.font.SysFont('Arial', 48, bold=True)
        self.game_over_subtitle_font = pygame.font.SysFont('Arial', 28)
        self.stats_font = pygame.font.SysFont('Arial', 20)
        self.game_objects = []
        self.step = 0
        self.game_over = False
        self.result = ""
        self.debug = debug
        # Add play/pause state
        self.paused = True
        self.last_time = 0

        # For parsed game result data
        self.winner = None
        self.win_reason = None
        self.p1_tanks = 0
        self.p2_tanks = 0
        self.max_steps = 0

        # Load assets with error handling
        self.tank_images = {}
        self.load_images()

    def load_images(self):
        try:
            # Check if assets directory exists
            if not os.path.exists('assets'):
                print("Warning: 'assets' directory not found, creating it")
                os.makedirs('assets')

            # Load tank images for player 1 and 2
            for player_id in [1, 2]:
                image_path = f'assets/tank_{"red" if player_id == 1 else "blue"}.png'
                if not os.path.exists(image_path):
                    print(f"Warning: {image_path} not found, creating placeholder")
                    self.create_placeholder_tank(player_id, image_path)

                try:
                    img = pygame.image.load(image_path)
                    self.tank_images[player_id] = pygame.transform.scale(img, (self.cell_size, self.cell_size))
                    if self.debug:
                        print(f"Successfully loaded tank image for player {player_id}")
                except Exception as e:
                    print(f"Error loading tank image for player {player_id}: {e}")
                    self.create_placeholder_tank(player_id, None)

            # Load shell image
            try:
                self.shell_image = pygame.transform.scale(
                    pygame.image.load('assets/shell.png'),
                    (self.cell_size // 2, self.cell_size // 2)
                )
            except:
                print("Warning: shell.png not found, creating placeholder")
                self.shell_image = self.create_placeholder_image((self.cell_size // 2, self.cell_size // 2), YELLOW,
                                                                 True)

            # Load mine image
            try:
                self.mine_image = pygame.transform.scale(
                    pygame.image.load('assets/mine.png'),
                    (self.cell_size // 2, self.cell_size // 2)
                )
            except:
                print("Warning: mine.png not found, creating placeholder")
                self.mine_image = self.create_placeholder_image((self.cell_size // 2, self.cell_size // 2), RED, True)

            # Load wall image
            try:
                self.wall_image = pygame.transform.scale(
                    pygame.image.load('assets/wall.png'),
                    (self.cell_size, self.cell_size)
                )
            except:
                print("Warning: wall.png not found, creating placeholder")
                self.wall_image = self.create_placeholder_image((self.cell_size, self.cell_size), BROWN)

        except Exception as e:
            print(f"Error loading images: {e}")

    def create_placeholder_tank(self, player_id, save_path=None):
        color = RED if player_id == 1 else BLUE
        surface = pygame.Surface((self.cell_size, self.cell_size), pygame.SRCALPHA)

        # Fill with player color
        pygame.draw.rect(surface, color, (0, 0, self.cell_size, self.cell_size))

        # Add direction indicator (triangle pointing up)
        pygame.draw.polygon(surface, WHITE, [
            (self.cell_size // 2, 5),
            (self.cell_size - 5, self.cell_size // 2),
            (5, self.cell_size // 2)
        ])

        self.tank_images[player_id] = surface

        # Save placeholder if needed
        if save_path:
            pygame.image.save(surface, save_path)

        return surface

    def create_placeholder_image(self, size, color, circle=False):
        surface = pygame.Surface(size, pygame.SRCALPHA)
        if circle:
            pygame.draw.circle(surface, color, (size[0] // 2, size[1] // 2), size[0] // 2)
        else:
            pygame.draw.rect(surface, color, (0, 0, size[0], size[1]))
        return surface

    def read_game_state(self):
        try:
            with open(self.game_state_file, 'r') as file:
                all_steps = []
                current_step = None

                for line in file:
                    line = line.strip()

                    if line.startswith("STEP"):
                        if current_step is not None:
                            all_steps.append(current_step)
                        current_step = {"objects": []}
                        self.step = int(line.split()[1])
                        current_step["step"] = self.step

                    elif line.startswith("GAME_OVER"):
                        self.game_over = True

                    elif self.game_over and not line.startswith("END_STEP"):
                        self.result = line
                        self.parse_game_result(line)

                    elif line == "END_STEP":
                        if current_step is not None:
                            all_steps.append(current_step)

                    elif current_step is not None and line[0].isdigit():
                        # This is the board dimensions line
                        dimensions = line.split()
                        if len(dimensions) == 2:
                            self.width = int(dimensions[0])
                            self.height = int(dimensions[1])
                            current_step["width"] = self.width
                            current_step["height"] = self.height

                    elif current_step is not None:
                        # This is an object data line
                        parts = line.split(',')
                        if len(parts) > 0:
                            obj_type = parts[0]
                            if obj_type in [ObjectType.TANK, ObjectType.SHELL, ObjectType.MINE, ObjectType.WALL]:
                                current_step["objects"].append(parts)
                                if self.debug and obj_type == ObjectType.TANK:
                                    print(f"Found tank: {parts}")

                # Add the last step if it wasn't added
                if current_step is not None and current_step not in all_steps:
                    all_steps.append(current_step)

                if self.debug:
                    print(f"Loaded {len(all_steps)} game states")
                    if all_steps:
                        print(f"First state has {len(all_steps[0]['objects'])} objects")

                return all_steps

        except Exception as e:
            print(f"Error reading game state file: {e}")
            return []

    def parse_game_result(self, result):
        """Parse the game result string to extract information about the winner and stats"""
        if "Player 1 won" in result:
            self.winner = 1
            # Extract number of tanks
            match = re.search(r"with (\d+) tanks", result)
            if match:
                self.p1_tanks = int(match.group(1))
            self.win_reason = "All opponent tanks destroyed"

        elif "Player 2 won" in result:
            self.winner = 2
            # Extract number of tanks
            match = re.search(r"with (\d+) tanks", result)
            if match:
                self.p2_tanks = int(match.group(1))
            self.win_reason = "All opponent tanks destroyed"

        elif "Tie, both players have zero tanks" in result:
            self.winner = 0
            self.win_reason = "Both players lost all tanks"

        elif "Tie, reached max steps" in result:
            self.winner = 0
            self.win_reason = "Maximum steps reached"
            # Extract steps and tank counts
            match_steps = re.search(r"max steps = (\d+)", result)
            if match_steps:
                self.max_steps = int(match_steps.group(1))

            match_p1 = re.search(r"player 1 has (\d+) tanks", result)
            match_p2 = re.search(r"player 2 has (\d+) tanks", result)
            if match_p1:
                self.p1_tanks = int(match_p1.group(1))
            if match_p2:
                self.p2_tanks = int(match_p2.group(1))

        elif "Tie, both players have zero shells" in result:
            self.winner = 0
            self.win_reason = "Both players out of ammunition"
            match = re.search(r"for (\d+) steps", result)
            if match:
                self.max_steps = int(match.group(1))

    def run(self):
        game_states = self.read_game_state()

        if not game_states:
            print("No game states found")
            return

        # Create the display window with the dimensions from the first game state
        self.width = game_states[0].get("width", 20)
        self.height = game_states[0].get("height", 20)

        # Add extra space for UI elements
        screen_width = self.width * self.cell_size
        screen_height = self.height * self.cell_size + 80  # Extra space for UI and controls
        self.screen = pygame.display.set_mode((screen_width, screen_height))
        pygame.display.set_caption("Tank Battle Visualizer")

        # Main visualization loop
        running = True
        current_state_idx = 0
        self.last_time = time.time()

        while running and current_state_idx < len(game_states):
            current_time = time.time()
            # Process events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        running = False
                    elif event.key == pygame.K_SPACE:
                        # Toggle play/pause
                        self.paused = not self.paused
                        # Reset time to avoid jumps when unpausing
                        self.last_time = current_time
                    elif event.key == pygame.K_RIGHT:
                        # Step forward one frame
                        current_state_idx = min(current_state_idx + 1, len(game_states) - 1)
                    elif event.key == pygame.K_LEFT:
                        # Step backward one frame
                        current_state_idx = max(current_state_idx - 1, 0)
                    elif event.key == pygame.K_UP:
                        # Step forward 5 frames
                        current_state_idx = min(current_state_idx + 5, len(game_states) - 1)
                    elif event.key == pygame.K_DOWN:
                        # Step backward 5 frames
                        current_state_idx = max(current_state_idx - 5, 0)
                    elif event.key == pygame.K_HOME:
                        # Go to first frame
                        current_state_idx = 0
                    elif event.key == pygame.K_END:
                        # Go to last frame
                        current_state_idx = len(game_states) - 1

            # Render current state
            # Pass a flag to indicate if this is the last state
            is_last_state = (current_state_idx == len(game_states) - 1)
            self.render_state(game_states[current_state_idx], is_last_state)

            # Display play/pause status and controls help
            status_text = "PAUSED" if self.paused else "PLAYING"
            status_surface = self.title_font.render(status_text, True, YELLOW if self.paused else GREEN)
            self.screen.blit(status_surface, (screen_width - status_surface.get_width() - 10,
                                              self.height * self.cell_size + 10))

            # Show controls help
            controls_text = "SPACE: Play/Pause | ←→: Step | ↑↓: Jump 3 | HOME/END: First/Last"
            controls_surface = self.font.render(controls_text, True, WHITE)
            self.screen.blit(controls_surface, (10, self.height * self.cell_size + 60))

            pygame.display.flip()

            # Automatically advance if not paused and not at the end
            if not self.paused and current_state_idx < len(game_states) - 1:
                # Only advance if enough time has passed
                if current_time - self.last_time >= self.delay:
                    current_state_idx += 1
                    self.last_time = current_time
            else:
                # When paused, limit the frame rate to avoid high CPU usage
                time.sleep(0.03)

        pygame.quit()

    def render_state(self, state, is_last_state=False):
        self.screen.fill(BLACK)

        # Draw grid
        for x in range(self.width):
            for y in range(self.height):
                rect = pygame.Rect(x * self.cell_size, y * self.cell_size, self.cell_size, self.cell_size)
                pygame.draw.rect(self.screen, GRAY, rect, 1)

        # Count objects by type
        object_counts = {'T': 0, '*': 0, '@': 0, '#': 0}

        # Draw objects
        for obj in state["objects"]:
            if len(obj) < 3:
                continue  # Skip malformed objects

            obj_type = obj[0]
            x = int(obj[1])
            y = int(obj[2])

            # Update object count
            if obj_type in object_counts:
                object_counts[obj_type] += 1

            if obj_type == ObjectType.TANK:
                if len(obj) < 6:  # Check if we have all required tank data
                    print(f"Warning: Incomplete tank data: {obj}")
                    continue

                player_id = int(obj[3])
                direction = int(obj[4])
                ammo = int(obj[5])

                # Make sure player_id is valid
                if player_id not in self.tank_images:
                    print(f"Warning: Invalid player_id {player_id} for tank")
                    player_id = 1  # Default to player 1

                # Draw the tank with rotation
                rotated_tank = pygame.transform.rotate(self.tank_images[player_id], -direction)
                tank_rect = rotated_tank.get_rect(center=(x * self.cell_size + self.cell_size // 2,
                                                          y * self.cell_size + self.cell_size // 2))
                self.screen.blit(rotated_tank, tank_rect.topleft)

                # Draw ammo count
                ammo_text = self.font.render(f"{ammo}", True, WHITE)
                self.screen.blit(ammo_text, (x * self.cell_size + 5, y * self.cell_size + 5))

            elif obj_type == ObjectType.SHELL:
                direction = int(obj[3]) if len(obj) > 3 else 0
                rotated_shell = pygame.transform.rotate(self.shell_image, -direction)
                shell_rect = rotated_shell.get_rect(center=(x * self.cell_size + self.cell_size // 2,
                                                            y * self.cell_size + self.cell_size // 2))
                self.screen.blit(rotated_shell, shell_rect.topleft)

            elif obj_type == ObjectType.MINE:
                mine_rect = self.mine_image.get_rect(center=(x * self.cell_size + self.cell_size // 2,
                                                             y * self.cell_size + self.cell_size // 2))
                self.screen.blit(self.mine_image, mine_rect.topleft)

            elif obj_type == ObjectType.WALL:
                wall_rect = self.wall_image.get_rect(topleft=(x * self.cell_size, y * self.cell_size))
                self.screen.blit(self.wall_image, wall_rect.topleft)

        # Draw step information
        step_text = self.title_font.render(f"Step: {state['step']}", True, WHITE)
        self.screen.blit(step_text, (10, self.height * self.cell_size + 10))

        # Draw object counts
        counts_text = self.font.render(
            f"Tanks: {object_counts['T']} | Shells: {object_counts['*']} | Mines: {object_counts['@']} | Walls: {object_counts['#']}",
            True, WHITE)
        self.screen.blit(counts_text, (10, self.height * self.cell_size + 40))

        # Draw game over result if applicable - now checks if this is the last state
        if self.game_over and is_last_state:
            self.draw_game_over_screen()

    def draw_game_over_screen(self):
        """Draw an attractive game over screen with results and statistics"""
        # Create a semi-transparent overlay
        overlay = pygame.Surface((self.screen.get_width(), self.screen.get_height()), pygame.SRCALPHA)
        overlay.fill((0, 0, 0, 180))  # Semi-transparent black
        self.screen.blit(overlay, (0, 0))

        # Calculate center position for the overlay
        screen_width = self.screen.get_width()
        screen_height = self.screen.get_height()
        center_x = screen_width // 2
        center_y = screen_height // 2 - 50  # Slightly above center

        # Create a background panel for the game over text
        panel_width = screen_width - 80
        panel_height = 300
        panel_rect = pygame.Rect(
            (screen_width - panel_width) // 2,
            (screen_height - panel_height) // 2 - 30,
            panel_width,
            panel_height
        )

        # Draw the panel with a border
        pygame.draw.rect(self.screen, (50, 50, 50), panel_rect)
        pygame.draw.rect(self.screen, GOLD, panel_rect, 3)

        # Draw "GAME OVER" text
        game_over_text = self.game_over_font.render("GAME OVER", True, GOLD)
        self.screen.blit(game_over_text,
                         (center_x - game_over_text.get_width() // 2,
                          panel_rect.top + 20))

        # Draw winner text with appropriate color
        if self.winner == 1:
            winner_color = RED
            winner_text = "PLAYER 1 WINS!"
        elif self.winner == 2:
            winner_color = BLUE
            winner_text = "PLAYER 2 WINS!"
        else:
            winner_color = SILVER
            winner_text = "TIE GAME"

        winner_surface = self.game_over_subtitle_font.render(winner_text, True, winner_color)
        self.screen.blit(winner_surface,
                         (center_x - winner_surface.get_width() // 2,
                          panel_rect.top + 80))

        # Draw reason text
        if self.win_reason:
            reason_surface = self.stats_font.render(self.win_reason, True, WHITE)
            self.screen.blit(reason_surface,
                             (center_x - reason_surface.get_width() // 2,
                              panel_rect.top + 120))

        # Draw tank statistics
        y_offset = panel_rect.top + 160

        # Player 1 stats with red color
        p1_text = self.stats_font.render(f"Player 1 Tanks: {self.p1_tanks}", True, RED)
        self.screen.blit(p1_text, (center_x - p1_text.get_width() // 2, y_offset))

        # Player 2 stats with blue color
        p2_text = self.stats_font.render(f"Player 2 Tanks: {self.p2_tanks}", True, BLUE)
        self.screen.blit(p2_text, (center_x - p2_text.get_width() // 2, y_offset + 30))

        # Draw steps info if applicable
        if self.max_steps > 0:
            steps_text = self.stats_font.render(f"Steps: {self.step}/{self.max_steps}", True, WHITE)
            self.screen.blit(steps_text, (center_x - steps_text.get_width() // 2, y_offset + 60))

        # Draw instruction to continue
        continue_text = self.font.render("Press ESC to exit", True, SILVER)
        self.screen.blit(continue_text,
                         (center_x - continue_text.get_width() // 2,
                          panel_rect.bottom - 30))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Tank Battle Visualizer')
    parser.add_argument('game_state_file', help='Path to game state file')
    parser.add_argument('--cell-size', type=int, default=40, help='Size of each cell in pixels')
    parser.add_argument('--delay', type=float, default=0.2, help='Delay between steps in seconds')
    parser.add_argument('--debug', action='store_true', help='Enable debug output')

    args = parser.parse_args()

    visualizer = TankVisualizer(args.game_state_file, args.cell_size, args.delay, args.debug)
    visualizer.run()
