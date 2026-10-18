import os
import math
from PIL import Image, ImageDraw

os.makedirs("assets/sprites", exist_ok=True)

C_INK = (16, 12, 20, 255)
C_TRANS = (0, 0, 0, 0)

# Master Palettes
PAL_DEVIL_RED = [
    (255, 120, 100, 255), # Specular crimson
    (225,  35,  40, 255), # Red skin body
    (170,  15,  25, 255), # Muscle shadow
    (110,   8,  15, 255), # Deep contour
    ( 60,   4,   8, 255), # Core shadow
]

PAL_DEVIL_HORN = [
    (230, 220, 200, 255),
    (150, 140, 130, 255),
    ( 80,  70,  65, 255),
    ( 35,  30,  30, 255),
]

PAL_EMERALD_FIRE = [
    (235, 255, 225, 255), # White-hot green core
    ( 90, 255, 120, 255), # Bright neon emerald
    ( 30, 210,  75, 255), # Pure emerald
    ( 15, 140,  45, 255), # Deep forest shadow
    (  8,  70,  25, 255), # Dark shade
]

PAL_GOD_ROBE = [
    (255, 255, 255, 255),
    (245, 242, 235, 255),
    (220, 210, 195, 255),
    (175, 165, 150, 255),
    (115, 105,  95, 255),
]

PAL_GOLD = [
    (255, 254, 210, 255),
    (255, 218,  72, 255),
    (204, 156,  34, 255),
    (142,  94,  18, 255),
    ( 72,  44,   8, 255),
]

PAL_SKIN = [
    (255, 240, 222, 255),
    (245, 195, 150, 255),
    (198, 134,  88, 255),
    (148,  82,  52, 255),
    ( 95,  46,  28, 255),
]

PAL_ANGEL_BLUE = [
    (235, 248, 255, 255),
    (185, 220, 255, 255),
    (130, 180, 240, 255),
    ( 80, 130, 200, 255),
    ( 40,  75, 140, 255),
]

def draw_ink_poly(draw, points, fill_col, outline_col=C_INK, outline_width=2):
    draw.polygon(points, fill=fill_col, outline=outline_col, width=outline_width)

def draw_ink_circle(draw, bbox, fill_col, outline_col=C_INK, outline_width=2):
    draw.ellipse(bbox, fill=fill_col, outline=outline_col, width=outline_width)

# =========================================================================
# 1. EMPOWERED GREEN FLAMING EVIL CLONE ("CHRISTUS TENEBRIS" SUMMIT FORM)
# =========================================================================
def generate_green_clone_sprites():
    # 32x48 sprite
    frames = ["idle", "walk_0", "attack", "hurt", "kneel"]
    for f in frames:
        img = Image.new("RGBA", (36, 48), C_TRANS)
        draw = ImageDraw.Draw(img)

        # Emerald Hellfire Aura aura rays
        draw_ink_circle(draw, (3, 4, 33, 46), (30, 210, 75, 45), outline_col=(90, 255, 120, 90), outline_width=1)

        # Torso & Flowing Himation Mantle in Emerald Fire
        if f == "kneel":
            draw_ink_poly(draw, [(10, 24), (26, 24), (30, 46), (6, 46)], PAL_EMERALD_FIRE[3], C_INK, 2)
            draw_ink_poly(draw, [(12, 26), (24, 26), (28, 44), (8, 44)], PAL_EMERALD_FIRE[2], C_INK, 1)
            # Head kneeling
            draw_ink_circle(draw, (12, 14, 24, 26), PAL_SKIN[2], C_INK, 2)
            # Green blazing eye
            draw.point((15, 19), fill=PAL_EMERALD_FIRE[0])
            draw.point((16, 19), fill=PAL_EMERALD_FIRE[1])
        elif f == "attack":
            # Extended strike pose
            draw_ink_poly(draw, [(10, 16), (26, 16), (32, 45), (4, 45)], PAL_EMERALD_FIRE[3], C_INK, 2)
            draw_ink_poly(draw, [(12, 18), (24, 18), (28, 42), (8, 42)], PAL_EMERALD_FIRE[2], C_INK, 1)
            # Head
            draw_ink_circle(draw, (12, 6, 24, 18), PAL_SKIN[2], C_INK, 2)
            # Blazing eyes
            draw.point((14, 11), fill=PAL_EMERALD_FIRE[0])
            draw.point((15, 11), fill=PAL_EMERALD_FIRE[1])
            # Extended arm casting emerald flame
            draw_ink_poly(draw, [(24, 18), (34, 16), (34, 22), (24, 22)], PAL_SKIN[2], C_INK, 2)
            # Emerald flame blast
            draw_ink_circle(draw, (31, 14, 36, 24), PAL_EMERALD_FIRE[0], outline_col=PAL_EMERALD_FIRE[1], outline_width=2)
        else:
            # Stand / Walk
            draw_ink_poly(draw, [(10, 16), (26, 16), (29, 45), (7, 45)], PAL_EMERALD_FIRE[3], C_INK, 2)
            draw_ink_poly(draw, [(12, 18), (24, 18), (27, 42), (9, 42)], PAL_EMERALD_FIRE[2], C_INK, 1)
            # Dark mantle folds
            draw_ink_poly(draw, [(11, 22), (23, 22), (25, 38), (13, 38)], (20, 28, 22, 255), C_INK, 1)
            # Head
            draw_ink_circle(draw, (12, 6, 24, 18), PAL_SKIN[2], C_INK, 2)
            # Dark auburn hair with emerald highlights
            draw_ink_circle(draw, (11, 5, 25, 13), (25, 50, 30, 255), C_INK, 1)
            # Glowing green eyes
            draw.point((15, 11), fill=PAL_EMERALD_FIRE[0])
            draw.point((16, 11), fill=PAL_EMERALD_FIRE[1])
            draw.point((20, 11), fill=PAL_EMERALD_FIRE[0])
            draw.point((21, 11), fill=PAL_EMERALD_FIRE[1])

        img.save(f"assets/sprites/spr_clone_green_{f}.png")
    print("Generated spr_clone_green sprites!")

# =========================================================================
# 2. THE DEVIL (LUCIFER / SATAN) - 48x48 CAPCOM CPS-2 SPRITES
# =========================================================================
def generate_devil_sprites():
    # 48x48 canvas
    states = ["idle", "walk_0", "attack", "tail_whip", "hurt", "die"]
    for s in states:
        img = Image.new("RGBA", (48, 48), C_TRANS)
        draw = ImageDraw.Draw(img)

        if s == "die":
            # Dissolving into dark hellfire embers on ground
            draw_ink_poly(draw, [(12, 38), (36, 38), (40, 46), (8, 46)], PAL_DEVIL_RED[4], C_INK, 2)
            draw_ink_poly(draw, [(16, 39), (32, 39), (35, 45), (13, 45)], (255, 140, 30, 255), C_INK, 1)
            # Smoke & sparks
            draw.point((18, 32), fill=(255, 200, 50, 255))
            draw.point((26, 28), fill=(255, 100, 30, 255))
            draw.point((30, 34), fill=(255, 220, 90, 255))
            img.save(f"assets/sprites/spr_devil_{s}.png")
            continue

        # Bat Wings on back
        draw_ink_poly(draw, [(24, 18), (8, 10), (12, 24), (20, 22)], (45, 10, 18, 255), C_INK, 2)
        draw_ink_poly(draw, [(24, 18), (40, 10), (36, 24), (28, 22)], (45, 10, 18, 255), C_INK, 2)

        # Muscular Red Demon Body
        draw_ink_poly(draw, [(18, 16), (30, 16), (28, 34), (20, 34)], PAL_DEVIL_RED[1], C_INK, 2)
        # Pecs & Abdominal definition
        draw_ink_poly(draw, [(19, 18), (24, 18), (24, 23), (19, 23)], PAL_DEVIL_RED[0], C_INK, 1)
        draw_ink_poly(draw, [(24, 18), (29, 18), (29, 23), (24, 23)], PAL_DEVIL_RED[0], C_INK, 1)
        draw_ink_poly(draw, [(20, 24), (28, 24), (27, 32), (21, 32)], PAL_DEVIL_RED[2], C_INK, 1)

        # Cloven Hooves & Legs
        draw_ink_poly(draw, [(18, 34), (23, 34), (21, 46), (16, 46)], PAL_DEVIL_RED[2], C_INK, 2)
        draw_ink_poly(draw, [(25, 34), (30, 34), (32, 46), (27, 46)], PAL_DEVIL_RED[2], C_INK, 2)
        # Black hooves
        draw_ink_poly(draw, [(15, 44), (22, 44), (21, 47), (14, 47)], (25, 20, 25, 255), C_INK, 1)
        draw_ink_poly(draw, [(26, 44), (33, 44), (32, 47), (25, 47)], (25, 20, 25, 255), C_INK, 1)

        # Head & Face
        draw_ink_circle(draw, (18, 6, 30, 18), PAL_DEVIL_RED[1], C_INK, 2)
        # Demon Ram Horns (Curving outward and up)
        draw_ink_poly(draw, [(19, 9), (13, 3), (17, 2), (21, 8)], PAL_DEVIL_HORN[2], C_INK, 1)
        draw_ink_poly(draw, [(29, 9), (35, 3), (31, 2), (27, 8)], PAL_DEVIL_HORN[2], C_INK, 1)
        # Horn tips
        draw_ink_poly(draw, [(13, 3), (10, 0), (14, 1)], PAL_DEVIL_HORN[0], C_INK, 1)
        draw_ink_poly(draw, [(35, 3), (38, 0), (34, 1)], PAL_DEVIL_HORN[0], C_INK, 1)

        # Glowing Yellow Slit Eyes & Pointed Goatee
        draw.point((21, 11), fill=(255, 240, 50, 255))
        draw.point((22, 11), fill=(255, 200, 30, 255))
        draw.point((26, 11), fill=(255, 240, 50, 255))
        draw.point((27, 11), fill=(255, 200, 30, 255))
        # Black goatee
        draw_ink_poly(draw, [(22, 17), (26, 17), (24, 21)], (20, 15, 20, 255), C_INK, 1)

        # THE DEVIL'S TAIL & ATTACK STATES
        if s == "tail_whip":
            # The Tail Whip Secret Attack! Tail lashes forward 24 pixels with barbed arrowhead!
            draw_ink_poly(draw, [(20, 32), (10, 30), (2, 24), (0, 14)], PAL_DEVIL_RED[2], C_INK, 2)
            # Barbed arrowhead whip tip
            draw_ink_poly(draw, [(0, 14), (6, 10), (0, 6)], (255, 50, 60, 255), C_INK, 2)
            # Whip flash streak
            draw.line([(0, 14), (16, 12), (32, 20)], fill=(255, 200, 60, 220), width=2)
            # Pitchfork held in rear hand
            draw_ink_poly(draw, [(30, 20), (44, 18), (44, 22), (30, 22)], (140, 95, 25, 255), C_INK, 1)
        elif s == "attack":
            # Thrusting Pitchfork / Trident forward
            draw_ink_poly(draw, [(18, 22), (4, 20), (4, 24), (18, 24)], (140, 95, 25, 255), C_INK, 2)
            # Golden Trident Prongs
            draw_ink_poly(draw, [(4, 16), (0, 16), (0, 28), (4, 28)], PAL_GOLD[1], C_INK, 2)
            draw_ink_poly(draw, [(0, 16), (-2, 12), (2, 12)], PAL_GOLD[0], C_INK, 1)
            draw_ink_poly(draw, [(0, 22), (-3, 20), (-3, 24)], PAL_GOLD[0], C_INK, 1)
            draw_ink_poly(draw, [(0, 28), (-2, 32), (2, 32)], PAL_GOLD[0], C_INK, 1)
            # Idle curled tail behind
            draw_ink_poly(draw, [(28, 32), (36, 34), (40, 28), (38, 24)], PAL_DEVIL_RED[2], C_INK, 2)
            draw_ink_poly(draw, [(38, 24), (44, 22), (40, 20)], (255, 50, 60, 255), C_INK, 1)
        else:
            # Idle / Walk / Hurt: Tail curled behind
            draw_ink_poly(draw, [(28, 32), (38, 36), (42, 30), (38, 26)], PAL_DEVIL_RED[2], C_INK, 2)
            # Barbed arrowhead tip
            draw_ink_poly(draw, [(38, 26), (44, 24), (41, 20)], (255, 50, 60, 255), C_INK, 1)
            # Pitchfork in hand
            draw_ink_poly(draw, [(28, 16), (36, 12), (38, 44), (35, 44)], (140, 95, 25, 255), C_INK, 1)
            draw_ink_poly(draw, [(33, 10), (42, 8), (40, 14), (32, 14)], PAL_GOLD[1], C_INK, 1)

        img.save(f"assets/sprites/spr_devil_{s}.png")
    print("Generated spr_devil sprites!")

# =========================================================================
# 3. "GOD" (THE DIVINE ILLUSION DESCENDING ON RAINBOW BRIDGE) - 40x56 SPRITE
# =========================================================================
def generate_god_sprite():
    img = Image.new("RGBA", (40, 56), C_TRANS)
    draw = ImageDraw.Draw(img)

    # Radiant Golden Halo with Solar Rays
    draw_ink_circle(draw, (4, 2, 36, 34), PAL_GOLD[1], outline_col=PAL_GOLD[0], outline_width=2)
    draw_ink_circle(draw, (7, 5, 33, 31), (255, 255, 230, 180), outline_col=PAL_GOLD[1], outline_width=1)

    # Flowing White and Gold Patriarch Robes
    draw_ink_poly(draw, [(12, 22), (28, 22), (34, 54), (6, 54)], PAL_GOD_ROBE[1], C_INK, 2)
    # Gold Hem Trim & Stole
    draw_ink_poly(draw, [(16, 22), (20, 22), (21, 54), (17, 54)], PAL_GOLD[1], C_INK, 1)
    draw_ink_poly(draw, [(20, 22), (24, 22), (23, 54), (19, 54)], PAL_GOLD[2], C_INK, 1)
    draw_ink_poly(draw, [(7, 50), (33, 50), (34, 54), (6, 54)], PAL_GOLD[1], C_INK, 1)

    # Head & Long Pure White Beard
    draw_ink_circle(draw, (14, 10, 26, 22), PAL_SKIN[1], C_INK, 2)
    # Eyes
    draw.point((17, 14), fill=(70, 120, 200, 255))
    draw.point((23, 14), fill=(70, 120, 200, 255))
    # Majestic Long White Beard
    draw_ink_poly(draw, [(13, 16), (27, 16), (25, 34), (20, 38), (15, 34)], (255, 255, 255, 255), C_INK, 2)
    draw_ink_poly(draw, [(16, 20), (24, 20), (22, 32), (18, 32)], (235, 235, 240, 255), C_INK, 1)

    # Welcoming Outstretched Hands Offering Mercy
    draw_ink_poly(draw, [(8, 26), (2, 24), (2, 28), (8, 29)], PAL_SKIN[1], C_INK, 1)
    draw_ink_poly(draw, [(32, 26), (38, 24), (38, 28), (32, 29)], PAL_SKIN[1], C_INK, 1)

    img.save("assets/sprites/spr_god_descend.png")
    print("Generated spr_god_descend sprite!")

# =========================================================================
# 4. SINGING ASCENDING ANGELS - 36x40 SPRITES
# =========================================================================
def generate_angel_sprites():
    for f in [0, 1]:
        img = Image.new("RGBA", (36, 40), C_TRANS)
        draw = ImageDraw.Draw(img)

        wing_y_offset = -2 if f == 0 else 2

        # White Feathered Wings
        draw_ink_poly(draw, [(18, 16), (2, 6 + wing_y_offset), (6, 22 + wing_y_offset), (16, 20)], (255, 255, 255, 255), C_INK, 2)
        draw_ink_poly(draw, [(18, 16), (34, 6 + wing_y_offset), (30, 22 + wing_y_offset), (20, 20)], (255, 255, 255, 255), C_INK, 2)

        # Sky Blue Celestial Robes
        draw_ink_poly(draw, [(12, 16), (24, 16), (28, 38), (8, 38)], PAL_ANGEL_BLUE[1], C_INK, 2)
        draw_ink_poly(draw, [(14, 18), (22, 18), (25, 36), (11, 36)], PAL_ANGEL_BLUE[0], C_INK, 1)

        # Golden Halo
        draw_ink_circle(draw, (11, 2, 25, 8), PAL_GOLD[1], outline_col=PAL_GOLD[0], outline_width=1)

        # Head & Singing Face
        draw_ink_circle(draw, (13, 6, 23, 16), PAL_SKIN[1], C_INK, 2)
        # Golden Auburn Hair
        draw_ink_circle(draw, (12, 5, 24, 11), PAL_GOLD[2], C_INK, 1)
        # Open Singing Mouth
        if f == 0:
            draw_ink_circle(draw, (17, 11, 19, 14), (180, 40, 50, 255), C_INK, 1)
        else:
            # Holding Golden Harp
            draw_ink_poly(draw, [(18, 14), (26, 12), (28, 24), (20, 22)], PAL_GOLD[1], C_INK, 1)

        img.save(f"assets/sprites/spr_angel_sing_{f}.png")
    print("Generated spr_angel_sing sprites!")

# =========================================================================
# 5. JESUS PLAYING SKEE-BALL IN HEAVEN & CHEERING ANGELS
# =========================================================================
def generate_skeeball_sprites():
    # Jesus bowling pose (36x48)
    img = Image.new("RGBA", (36, 48), C_TRANS)
    draw = ImageDraw.Draw(img)

    # Rolled-up cream tunic with crimson mantle tied at waist
    draw_ink_poly(draw, [(10, 18), (24, 18), (26, 44), (8, 44)], (245, 242, 235, 255), C_INK, 2)
    # Crimson sash tied at waist
    draw_ink_poly(draw, [(9, 26), (25, 26), (25, 31), (9, 31)], (195, 28, 38, 255), C_INK, 1)

    # Bending bowling posture - Right arm extended forward rolling wooden ball
    draw_ink_poly(draw, [(18, 22), (32, 30), (31, 35), (17, 27)], PAL_SKIN[1], C_INK, 2)
    # Wooden Skee-ball in hand / rolling
    draw_ink_circle(draw, (29, 32, 35, 38), (185, 125, 60, 255), outline_col=C_INK, outline_width=1)

    # Head with joyful smile & Golden Halo
    draw_ink_circle(draw, (10, 2, 26, 10), PAL_GOLD[1], outline_col=PAL_GOLD[0], outline_width=1)
    draw_ink_circle(draw, (12, 6, 24, 18), PAL_SKIN[1], C_INK, 2)
    # Auburn hair & beard
    draw_ink_circle(draw, (11, 5, 25, 12), (142, 86, 46, 255), C_INK, 1)
    draw_ink_poly(draw, [(14, 14), (22, 14), (20, 19), (16, 19)], (142, 86, 46, 255), C_INK, 1)
    # Smile
    draw.arc((16, 11, 20, 15), 0, 180, fill=C_INK, width=1)

    img.save("assets/sprites/spr_skeeball_jesus.png")

    # Cheering Angel with Golden Pom-Poms (36x44)
    for c in [0, 1]:
        cimg = Image.new("RGBA", (36, 44), C_TRANS)
        cdraw = ImageDraw.Draw(cimg)

        # Wings
        draw_ink_poly(cdraw, [(18, 18), (2, 8), (6, 24), (16, 22)], (255, 255, 255, 255), C_INK, 2)
        draw_ink_poly(cdraw, [(18, 18), (34, 8), (30, 24), (20, 22)], (255, 255, 255, 255), C_INK, 2)

        # Robes
        draw_ink_poly(cdraw, [(12, 18), (24, 18), (27, 42), (9, 42)], PAL_ANGEL_BLUE[1], C_INK, 2)

        # Halo
        draw_ink_circle(cdraw, (11, 2, 25, 8), PAL_GOLD[1], outline_col=PAL_GOLD[0], outline_width=1)

        # Head
        draw_ink_circle(cdraw, (13, 8, 23, 18), PAL_SKIN[1], C_INK, 2)

        # Arms raised waving Golden Pom-Poms!
        pom_y = 6 if c == 0 else 2
        draw_ink_poly(cdraw, [(12, 20), (4, pom_y + 8), (6, pom_y + 12), (13, 22)], PAL_SKIN[1], C_INK, 1)
        draw_ink_poly(cdraw, [(24, 20), (32, pom_y + 8), (30, pom_y + 12), (23, 22)], PAL_SKIN[1], C_INK, 1)
        # Golden Pom-Poms
        draw_ink_circle(cdraw, (0, pom_y, 10, pom_y + 10), PAL_GOLD[1], outline_col=PAL_GOLD[0], outline_width=1)
        draw_ink_circle(cdraw, (26, pom_y, 36, pom_y + 10), PAL_GOLD[1], outline_col=PAL_GOLD[0], outline_width=1)

        cimg.save(f"assets/sprites/spr_angel_cheer_{c}.png")
    print("Generated skee-ball & cheering angel sprites!")

if __name__ == "__main__":
    generate_green_clone_sprites()
    generate_devil_sprites()
    generate_god_sprite()
    generate_angel_sprites()
    generate_skeeball_sprites()
    print("ALL MISSION 5 SPRITES GENERATED SUCCESSFULLY!")

