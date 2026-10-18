import os
import math
from PIL import Image, ImageDraw

os.makedirs("assets/sprites", exist_ok=True)

# 16-bit Master Palette definitions
PAL_WHITE_ROBE = [
    (255, 255, 255, 255),
    (245, 242, 235, 255),
    (210, 205, 195, 255),
    (165, 158, 150, 255),
    (110, 102,  98, 255),
    ( 40,  35,  40, 255),
]

PAL_CRIMSON = [
    (255, 150, 160, 255),
    (235,  45,  60, 255),
    (175,  25,  35, 255),
    (115,  15,  22, 255),
    ( 65,   8,  14, 255),
    ( 25,   5,   8, 255),
]

PAL_GOLD = [
    (255, 252, 200, 255),
    (255, 215,  70, 255),
    (205, 155,  35, 255),
    (145,  95,  20, 255),
    ( 75,  45,  10, 255),
    ( 25,  18,   5, 255),
]

PAL_SKIN = [
    (255, 235, 210, 255),
    (240, 195, 155, 255),
    (205, 150, 110, 255),
    (160, 105,  70, 255),
    (105,  60,  35, 255),
    ( 45,  25,  15, 255),
]

PAL_HAIR = [
    (185, 125,  70, 255),
    (125,  75,  40, 255),
    ( 75,  40,  20, 255),
    ( 40,  20,  10, 255),
]

PAL_STEEL = [
    (255, 255, 255, 255),
    (225, 230, 240, 255),
    (165, 175, 195, 255),
    (105, 115, 135, 255),
    ( 55,  60,  75, 255),
    ( 20,  22,  28, 255),
]

PAL_BRONZE = [
    (255, 240, 180, 255),
    (220, 170,  70, 255),
    (170, 115,  40, 255),
    (110,  68,  22, 255),
    ( 55,  32,  10, 255),
]

def draw_limb_3tone(draw, p1, p2, r1, r2, pal, outline_col=(20, 18, 25, 255)):
    draw.line([p1, p2], fill=outline_col, width=int((r1 + r2) + 4))
    draw.ellipse([p1[0]-r1-2, p1[1]-r1-2, p1[0]+r1+2, p1[1]+r1+2], fill=outline_col)
    draw.ellipse([p2[0]-r2-2, p2[1]-r2-2, p2[0]+r2+2, p2[1]+r2+2], fill=outline_col)

    draw.line([p1, p2], fill=pal[3] if len(pal)>3 else pal[-1], width=int(r1 + r2))
    draw.ellipse([p1[0]-r1, p1[1]-r1, p1[0]+r1, p1[1]+r1], fill=pal[3] if len(pal)>3 else pal[-1])
    draw.ellipse([p2[0]-r2, p2[1]-r2, p2[0]+r2, p2[1]+r2], fill=pal[3] if len(pal)>3 else pal[-1])

    draw.line([p1, p2], fill=pal[1], width=int((r1 + r2) * 0.7))
    draw.ellipse([p1[0]-r1*0.7, p1[1]-r1*0.7, p1[0]+r1*0.7, p1[1]+r1*0.7], fill=pal[1])
    draw.ellipse([p2[0]-r2*0.7, p2[1]-r2*0.7, p2[0]+r2*0.7, p2[1]+r2*0.7], fill=pal[1])

    hp1 = (p1[0], p1[1] - r1 * 0.35)
    hp2 = (p2[0], p2[1] - r2 * 0.35)
    draw.line([hp1, hp2], fill=pal[0], width=max(1, int((r1 + r2) * 0.25)))

# --- 1. MONEY CHANGER (Mission 3 Bazaar Enemy) ---
def create_money_changer(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 68, 38

    if is_hurt:
        cx -= 6

    # Turban with gold brooch
    draw.ellipse([cx - 16, head_y - 20, cx + 16, head_y + 2], fill=(245, 240, 230, 255), outline=(50, 45, 40, 255), width=2)
    draw.ellipse([cx - 5, head_y - 14, cx + 5, head_y - 4], fill=PAL_GOLD[0], outline=PAL_GOLD[4])

    # Face with pointed beard and sly mustache
    draw.ellipse([cx - 12, head_y - 10, cx + 12, head_y + 10], fill=PAL_SKIN[2])
    draw.polygon([(cx - 8, head_y + 8), (cx + 8, head_y + 8), (cx, head_y + 22)], fill=PAL_HAIR[2])
    draw.point((cx - 4, head_y - 2), fill=(20, 20, 20, 255))
    draw.point((cx + 4, head_y - 2), fill=(20, 20, 20, 255))

    # Rotund Tunic with Saffron & Royal Tyrian Stripes
    draw.polygon([(cx - 18, torso_y - 6), (cx + 18, torso_y - 6), (cx + 22, torso_y + 36), (cx - 22, torso_y + 36)], fill=(215, 175, 45, 255), outline=(65, 45, 15, 255), width=2)
    for sx in range(cx - 14, cx + 16, 7):
        draw.line([(sx, torso_y - 4), (sx, torso_y + 34)], fill=(120, 20, 95, 255), width=3)

    # Legs & Slippers
    stride = math.sin(walk_phase * math.pi / 2.0) * 11.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 34), (cx - 8 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 34), (cx + 8 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)

    # Heavy Leather Coin Bag
    bag_x = cx + 24 if not is_attack else cx - 36
    bag_y = torso_y + 16 if not is_attack else torso_y - 10
    draw_limb_3tone(draw, (cx + 14, torso_y), (bag_x, bag_y), 6, 4, PAL_SKIN)
    draw.ellipse([bag_x - 12, bag_y - 4, bag_x + 12, bag_y + 22], fill=(130, 80, 45, 255), outline=(50, 30, 15, 255), width=2)
    draw.line([(bag_x - 6, bag_y - 2), (bag_x + 6, bag_y - 2)], fill=PAL_GOLD[0], width=2)

    if is_attack:
        for dx, dy in [(-12, -8), (-22, 4), (-10, 16), (-18, -18)]:
            draw.ellipse([bag_x + dx - 4, bag_y + dy - 4, bag_x + dx + 4, bag_y + dy + 4], fill=PAL_GOLD[0], outline=PAL_GOLD[4])

    if is_hurt:
        draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 40, 40, 255), width=3)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

# --- 2. LEVITE ENFORCER (Mission 3 Palace of Caiaphas Enemy) ---
def create_levite_enforcer(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 128, 152
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 70, 38

    if is_hurt:
        cx -= 6

    # Conical Temple Bronze Helmet
    draw.polygon([(cx - 16, head_y - 24), (cx + 16, head_y - 24), (cx, head_y - 42)], fill=PAL_BRONZE[1], outline=PAL_BRONZE[4], width=2)
    draw.ellipse([cx - 14, head_y - 14, cx + 14, head_y + 10], fill=PAL_BRONZE[2], outline=PAL_BRONZE[4])

    # Bearded Temple Warrior Face
    draw.ellipse([cx - 10, head_y - 6, cx + 10, head_y + 10], fill=PAL_SKIN[2])
    draw.polygon([(cx - 10, head_y + 6), (cx + 10, head_y + 6), (cx, head_y + 22)], fill=PAL_HAIR[2])

    # Deep Celestial Blue & White Pleated Tunic
    draw.polygon([(cx - 20, torso_y - 8), (cx + 20, torso_y - 8), (cx + 22, torso_y + 34), (cx - 22, torso_y + 34)], fill=(28, 65, 125, 255), outline=(12, 28, 65, 255), width=2)
    draw.line([(cx - 18, torso_y - 4), (cx + 18, torso_y - 4)], fill=PAL_GOLD[0], width=3)
    draw.line([(cx, torso_y - 4), (cx, torso_y + 32)], fill=PAL_GOLD[1], width=2)

    # Greaved Legs
    stride = math.sin(walk_phase * math.pi / 2.0) * 11.0
    draw_limb_3tone(draw, (cx - 10, torso_y + 32), (cx - 10 + int(stride), ground_y - 4), 7, 5, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 10, torso_y + 32), (cx + 10 - int(stride), ground_y - 4), 7, 5, PAL_BRONZE)

    # Spiked Bronze Mace / Staff
    staff_x = cx + 28 if not is_attack else cx - 40
    draw.line([(staff_x, 14), (staff_x, ground_y)], fill=(120, 80, 45, 255), width=5)
    draw.ellipse([staff_x - 12, 10, staff_x + 12, 34], fill=PAL_BRONZE[0], outline=PAL_BRONZE[4], width=2)
    for dy in range(14, 32, 6):
        draw.line([(staff_x - 16, dy), (staff_x + 16, dy)], fill=PAL_BRONZE[1], width=2)

    if is_hurt:
        draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

# --- 3. BONUS STAGE VILLAGERS ---
def create_villager(frame_name, vtype="elder", run_phase=0, is_satisfied=False):
    W, H = 96, 128
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 6
    torso_y, head_y = 60, 32

    # Running legs
    stride = math.sin(run_phase * math.pi / 2.0) * 14.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 26), (cx - 8 + int(stride), ground_y - 4), 5, 4, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 26), (cx + 8 - int(stride), ground_y - 4), 5, 4, PAL_SKIN)

    if vtype == "elder":
        draw.ellipse([cx - 12, head_y - 12, cx + 12, head_y + 8], fill=PAL_SKIN[2])
        draw.polygon([(cx - 10, head_y + 4), (cx + 10, head_y + 4), (cx, head_y + 24)], fill=(240, 240, 245, 255))
        draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 18, torso_y + 26), (cx - 18, torso_y + 26)], fill=(130, 95, 60, 255))
    elif vtype == "woman":
        draw.ellipse([cx - 14, head_y - 14, cx + 14, head_y + 14], fill=(240, 240, 245, 255))
        draw.ellipse([cx - 10, head_y - 8, cx + 10, head_y + 8], fill=PAL_SKIN[1])
        draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 20, torso_y + 26), (cx - 20, torso_y + 26)], fill=(45, 125, 135, 255))
    elif vtype == "youth":
        draw.ellipse([cx - 10, head_y - 10, cx + 10, head_y + 8], fill=PAL_SKIN[1])
        draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 16, torso_y + 24), (cx - 16, torso_y + 24)], fill=(65, 135, 65, 255))
    else: # beggar
        draw.ellipse([cx - 10, head_y - 10, cx + 10, head_y + 8], fill=PAL_SKIN[3])
        draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 16, torso_y + 24), (cx - 16, torso_y + 24)], fill=(110, 100, 85, 255))

    if is_satisfied:
        draw.ellipse([cx - 8, torso_y, cx + 8, torso_y + 14], fill=(240, 180, 80, 255))
        for spx, spy in [(-16, head_y - 12), (16, head_y - 12), (0, head_y - 20)]:
            draw.ellipse([cx + spx - 3, spy - 3, cx + spx + 3, spy + 3], fill=(255, 255, 180, 255))

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

# --- 4. RELICS, MAGIC & PROJECTILES ---
def create_relics_and_magic():
    # Dark Orb (48x48)
    img = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r, col in [(22, (120, 20, 180, 60)), (16, (170, 40, 230, 140)), (10, (220, 120, 255, 210)), (4, (255, 255, 255, 255))]:
        d.ellipse([24 - r, 24 - r, 24 + r, 24 + r], fill=col)
    img.save("assets/sprites/spr_dark_orb.png", "PNG")

    # Dark Spike (64x96)
    img = Image.new("RGBA", (64, 96), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.polygon([(32, 6), (54, 90), (10, 90)], fill=(45, 15, 65, 255), outline=(180, 40, 240, 255), width=2)
    d.polygon([(32, 16), (42, 86), (22, 86)], fill=(120, 30, 170, 255))
    img.save("assets/sprites/spr_dark_spike.png", "PNG")

    # Blood Shockwave (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([4, 4, 60, 60], outline=PAL_CRIMSON[1], width=4)
    d.ellipse([12, 12, 52, 52], outline=PAL_CRIMSON[0], width=2)
    img.save("assets/sprites/spr_blood_shockwave.png", "PNG")

    # Caiaphas Lightning (64x384)
    img = Image.new("RGBA", (64, 384), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    points = [(32, 0)]
    for y in range(20, 380, 24):
        offset = 12 if (y // 24) % 2 == 0 else -12
        points.append((32 + offset, y))
    points.append((32, 384))
    d.line(points, fill=(180, 220, 255, 255), width=8)
    d.line(points, fill=(255, 255, 255, 255), width=3)
    img.save("assets/sprites/spr_caiaphas_lightning.png", "PNG")

    # Shout Wave (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r in range(12, 30, 8):
        d.arc([32 - r, 32 - r, 32 + r, 32 + r], -60, 60, fill=PAL_GOLD[0], width=3)
    img.save("assets/sprites/spr_shout_wave.png", "PNG")

    # Water Droplet (32x32)
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.polygon([(16, 4), (26, 24), (6, 24)], fill=(120, 210, 255, 240), outline=(255, 255, 255, 255), width=2)
    d.ellipse([6, 14, 26, 28], fill=(120, 210, 255, 240), outline=(255, 255, 255, 255), width=2)
    img.save("assets/sprites/spr_water_droplet.png", "PNG")

    # Golden Stylus Blade (56x32)
    img = Image.new("RGBA", (56, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.polygon([(6, 16), (46, 10), (52, 16), (46, 22)], fill=PAL_GOLD[0], outline=PAL_GOLD[4], width=2)
    img.save("assets/sprites/spr_golden_stylus_blade.png", "PNG")

    # Silver Pitcher (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.polygon([(22, 14), (42, 14), (48, 54), (16, 54)], fill=PAL_STEEL[1], outline=PAL_STEEL[5], width=2)
    d.arc([36, 18, 56, 46], 270, 90, fill=PAL_STEEL[0], width=3) # Handle
    img.save("assets/sprites/spr_silver_pitcher.png", "PNG")

    # Olive Wreath (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([10, 10, 54, 54], outline=(45, 125, 45, 255), width=6)
    for ang in range(0, 360, 30):
        rad = math.radians(ang)
        ox = int(32 + math.cos(rad) * 22)
        oy = int(32 + math.sin(rad) * 22)
        d.ellipse([ox - 3, oy - 3, ox + 3, oy + 3], fill=PAL_GOLD[0])
    img.save("assets/sprites/spr_olive_wreath.png", "PNG")

    # Sacred Chalice (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.polygon([(18, 12), (46, 12), (40, 34), (24, 34)], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
    d.line([(32, 34), (32, 50)], fill=PAL_GOLD[0], width=4)
    d.polygon([(18, 54), (46, 54), (32, 50)], fill=PAL_GOLD[2])
    img.save("assets/sprites/spr_sacred_chalice.png", "PNG")

    # Bitter Herbs (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.arc([16, 16, 48, 52], 180, 360, fill=(60, 160, 50, 255), width=5)
    d.ellipse([18, 14, 32, 28], fill=(90, 190, 70, 255))
    d.ellipse([32, 14, 46, 28], fill=(90, 190, 70, 255))
    img.save("assets/sprites/spr_bitter_herbs.png", "PNG")

    # Passover Matzot (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([10, 14, 54, 50], fill=(225, 195, 140, 255), outline=(130, 95, 50, 255), width=2)
    for x in range(18, 48, 8):
        for y in range(22, 44, 7):
            d.point((x, y), fill=(100, 65, 30, 255))
    img.save("assets/sprites/spr_passover_matzot.png", "PNG")

    # Holy Grail (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # Radiance
    for r in range(26, 0, -3):
        d.ellipse([32 - r, 32 - r, 32 + r, 32 + r], fill=(255, 240, 160, 15))
    d.polygon([(18, 12), (46, 12), (38, 36), (26, 36)], fill=PAL_GOLD[0], outline=PAL_GOLD[4], width=2)
    d.line([(32, 36), (32, 50)], fill=PAL_GOLD[1], width=5)
    d.ellipse([20, 48, 44, 56], fill=PAL_GOLD[0], outline=PAL_GOLD[4], width=2)
    # Wine ruby glow
    d.ellipse([22, 14, 42, 22], fill=PAL_CRIMSON[1])
    img.save("assets/sprites/spr_holy_grail.png", "PNG")

    # Roman Scroll (64x64)
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rectangle([14, 18, 50, 46], fill=(245, 235, 195, 255), outline=(110, 85, 45, 255), width=2)
    d.ellipse([26, 26, 38, 38], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5]) # Red imperial seal
    img.save("assets/sprites/spr_roman_scroll.png", "PNG")

    # Caesar Seal (56x56)
    img = Image.new("RGBA", (56, 56), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([6, 6, 50, 50], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=3)
    d.ellipse([14, 14, 42, 42], fill=PAL_GOLD[0])
    # Caesar profile silhouette
    d.polygon([(28, 18), (34, 26), (30, 36), (24, 34)], fill=PAL_GOLD[3])
    img.save("assets/sprites/spr_caesar_seal.png", "PNG")

    # Pilate Ewer (64x72)
    img = Image.new("RGBA", (64, 72), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.polygon([(20, 16), (44, 16), (50, 60), (14, 60)], fill=PAL_BRONZE[1], outline=PAL_BRONZE[4], width=2)
    d.arc([36, 20, 58, 52], 270, 90, fill=PAL_BRONZE[0], width=4)
    img.save("assets/sprites/spr_pilate_ewer.png", "PNG")

    # Censer Gold (72x72)
    img = Image.new("RGBA", (72, 72), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([16, 32, 56, 64], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
    d.polygon([(36, 8), (22, 32), (50, 32)], fill=PAL_GOLD[0])
    d.line([(36, 4), (36, 8)], fill=PAL_GOLD[4], width=2)
    img.save("assets/sprites/spr_censer_gold.png", "PNG")

    # Whip Head (56x56)
    img = Image.new("RGBA", (56, 56), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for dx, dy in [(12, 14), (28, 28), (44, 42)]:
        d.ellipse([dx - 5, dy - 5, dx + 5, dy + 5], fill=PAL_STEEL[0], outline=PAL_STEEL[4], width=2)
    d.line([(12, 14), (28, 28), (44, 42)], fill=PAL_STEEL[2], width=3)
    img.save("assets/sprites/spr_whip_head.png", "PNG")

    print("Generated all relics, magic and projectiles!")

# Run all generators
for p in [0, 1]:
    create_money_changer(f"spr_money_changer_walk_{p}", walk_phase=p)
create_money_changer("spr_money_changer_attack", is_attack=True)
create_money_changer("spr_money_changer_hurt", is_hurt=True)

for p in [0, 1]:
    create_levite_enforcer(f"spr_levite_enforcer_walk_{p}", walk_phase=p)
create_levite_enforcer("spr_levite_enforcer_attack", is_attack=True)
create_levite_enforcer("spr_levite_enforcer_hurt", is_hurt=True)

for vtype in ["elder", "woman", "youth", "beggar"]:
    create_villager(f"spr_villager_{vtype}_run_0", vtype=vtype, run_phase=0)
    create_villager(f"spr_villager_{vtype}_run_1", vtype=vtype, run_phase=1)
create_villager("spr_villager_satisfied", is_satisfied=True)

create_relics_and_magic()

print("All extra 16-bit arcade sprites created successfully!")

# --- 5. BOSS WALK & HURT FRAMES ---
def create_centurion_frames():
    W, H = 144, 176
    for walk_p, is_h, name in [(0, False, "spr_boss_walk_0"), (1, False, "spr_boss_walk_1"), (0, True, "spr_boss_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 78, 42
        if is_h:
            cx -= 6
        # Helmet & Crest
        draw.ellipse([cx - 28, head_y - 32, cx + 28, head_y - 14], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
        draw.ellipse([cx - 16, head_y - 16, cx + 16, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
        # Cuirass
        draw.polygon([(cx - 24, torso_y - 12), (cx + 24, torso_y - 12), (cx + 20, torso_y + 34), (cx - 20, torso_y + 34)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
        # Cloak
        draw.polygon([(cx - 22, torso_y - 8), (cx - 48, torso_y + 60), (cx - 18, ground_y - 12), (cx + 22, torso_y - 8)], fill=PAL_CRIMSON[2], outline=PAL_CRIMSON[5])
        # Legs
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 12, torso_y + 34), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw_limb_3tone(draw, (cx + 12, torso_y + 34), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        # Whip
        draw_limb_3tone(draw, (cx - 20, torso_y), (cx - 28, torso_y + 24), 8, 6, PAL_SKIN)
        draw.line([(cx - 28, torso_y + 24), (cx - 32, ground_y - 10)], fill=PAL_STEEL[2], width=3)
        if is_h:
            draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_temple_boss_frames():
    W, H = 160, 192
    for walk_p, is_h, name in [(0, False, "spr_temple_boss_walk_0"), (1, False, "spr_temple_boss_walk_1"), (0, True, "spr_temple_boss_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 82, 46
        if is_h:
            cx -= 6
        draw.ellipse([cx - 20, head_y - 24, cx + 20, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[5], width=2)
        draw.polygon([(cx - 26, torso_y - 12), (cx + 26, torso_y - 12), (cx + 22, torso_y + 38), (cx - 22, torso_y + 38)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 14, torso_y + 38), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw_limb_3tone(draw, (cx + 14, torso_y + 38), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw.line([(cx - 22, torso_y), (cx - 36, ground_y - 10)], fill=(140, 95, 45, 255), width=5)
        draw.ellipse([cx - 44, ground_y - 24, cx - 28, ground_y - 4], fill=PAL_GOLD[0], outline=PAL_GOLD[4])
        if is_h:
            draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_tribunal_boss_frames():
    W, H = 160, 192
    for walk_p, is_h, name in [(0, False, "spr_tribunal_walk_0"), (1, False, "spr_tribunal_walk_1"), (0, True, "spr_tribunal_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 82, 46
        if is_h:
            cx -= 6
        draw.polygon([(cx - 24, head_y - 32), (cx + 24, head_y - 32), (cx, head_y - 16)], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
        draw.ellipse([cx - 18, head_y - 18, cx + 18, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
        draw.polygon([(cx - 26, torso_y - 12), (cx + 26, torso_y - 12), (cx + 22, torso_y + 38), (cx - 22, torso_y + 38)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 14, torso_y + 38), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw_limb_3tone(draw, (cx + 14, torso_y + 38), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw.line([(cx + 24, torso_y - 50), (cx + 24, ground_y)], fill=(120, 75, 35, 255), width=5)
        draw.polygon([(cx + 19, torso_y - 50), (cx + 29, torso_y - 50), (cx + 24, torso_y - 74)], fill=PAL_STEEL[0], outline=PAL_STEEL[4])
        if is_h:
            draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_clone_boss_frames():
    W, H = 112, 144
    for walk_p, is_h, name in [(0, False, "spr_clone_walk_0"), (1, False, "spr_clone_walk_1"), (0, True, "spr_clone_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 68, 38
        if is_h:
            cx -= 5
        halo_r = 26
        for r in range(halo_r, 0, -2):
            draw.ellipse([cx - r, head_y - r - 4, cx + r, head_y + r - 4], fill=(140, 20, 220, 40))
        draw.ellipse([cx - 12, head_y - 14, cx + 12, head_y + 12], fill=(200, 195, 210, 255))
        draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 20, ground_y - 12), (cx - 20, ground_y - 12)], fill=(30, 28, 38, 255), outline=(12, 10, 18, 255))
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 8, torso_y + 24), (cx - 8 + int(stride), ground_y - 4), 7, 5, (190, 185, 200, 255))
        draw_limb_3tone(draw, (cx + 8, torso_y + 24), (cx + 8 - int(stride), ground_y - 4), 7, 5, (190, 185, 200, 255))
        if is_h:
            draw.line([(cx - 10, head_y - 4), (cx + 10, head_y + 4)], fill=(255, 40, 40, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_barabbas_frames():
    W, H = 152, 176
    for walk_p, is_h, name in [(0, False, "spr_barabbas_walk_0"), (1, False, "spr_barabbas_walk_1"), (0, True, "spr_barabbas_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 76, 40
        if is_h:
            cx -= 6
        draw.ellipse([cx - 24, head_y - 20, cx + 24, head_y + 26], fill=(45, 28, 16, 255))
        draw.ellipse([cx - 14, head_y - 10, cx + 14, head_y + 12], fill=PAL_SKIN[3])
        draw.polygon([(cx - 26, torso_y - 10), (cx + 26, torso_y - 10), (cx + 20, torso_y + 36), (cx - 20, torso_y + 36)], fill=PAL_SKIN[2], outline=PAL_SKIN[5], width=2)
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 12, torso_y + 36), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 12, torso_y + 36), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_SKIN)
        if is_h:
            draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 40, 40, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_caiaphas_frames():
    W, H = 128, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 70, 36
    cx -= 6
    draw.polygon([(cx - 18, head_y - 24), (cx + 18, head_y - 24), (cx + 14, head_y + 8), (cx - 14, head_y + 8)], fill=(240, 245, 255, 255), outline=PAL_GOLD[4], width=2)
    draw.polygon([(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 24, ground_y - 6), (cx - 24, ground_y - 6)], fill=(32, 85, 160, 255), outline=(12, 35, 75, 255), width=2)
    draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 40, 40, 255), width=3)
    img.save("assets/sprites/spr_caiaphas_hurt.png", "PNG")
    print("Generated: assets/sprites/spr_caiaphas_hurt.png")

def create_judas_frames():
    W, H = 112, 144
    for walk_p, is_h, name in [(0, False, "spr_judas_walk_0"), (1, False, "spr_judas_walk_1"), (0, True, "spr_judas_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 68, 38
        if is_h:
            cx -= 5
        draw.polygon([(cx - 18, head_y - 18), (cx + 18, head_y - 18), (cx + 14, head_y + 18), (cx - 14, head_y + 18)], fill=(65, 75, 45, 255))
        draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 22, torso_y + 36), (cx - 22, torso_y + 36)], fill=(85, 95, 55, 255))
        stride = math.sin(walk_p * math.pi / 2.0) * 12.0
        draw_limb_3tone(draw, (cx - 8, torso_y + 32), (cx - 10 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 8, torso_y + 32), (cx + 10 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)
        pouch_x = cx + 16
        draw.ellipse([pouch_x - 8, torso_y + 18, pouch_x + 8, torso_y + 32], fill=(120, 75, 40, 255))
        if is_h:
            draw.line([(cx - 10, head_y - 4), (cx + 10, head_y + 4)], fill=(240, 40, 40, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_praetorian_and_lictor_extra():
    # Praetorian Walk 1 & Hurt
    W, H = 128, 160
    for walk_p, is_h, name in [(1, False, "spr_praetorian_walk_1"), (0, True, "spr_praetorian_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 74, 40
        if is_h:
            cx -= 5
        draw.ellipse([cx - 20, head_y - 30, cx + 20, head_y - 14], fill=(125, 20, 120, 255))
        draw.ellipse([cx - 15, head_y - 16, cx + 15, head_y + 10], fill=(35, 32, 42, 255), outline=PAL_GOLD[1], width=2)
        draw.rectangle([cx - 18, torso_y - 8, cx + 18, torso_y + 28], fill=(32, 30, 38, 255), outline=PAL_GOLD[2], width=2)
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 10, torso_y + 28), (cx - 10 + int(stride), ground_y - 4), 7, 5, PAL_BRONZE)
        draw_limb_3tone(draw, (cx + 10, torso_y + 28), (cx + 10 - int(stride), ground_y - 4), 7, 5, PAL_BRONZE)
        sh_x = cx + 26
        draw.ellipse([sh_x - 14, torso_y - 14, sh_x + 14, torso_y + 38], fill=(28, 25, 36, 255), outline=PAL_GOLD[1], width=3)
        if is_h:
            draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

    # Lictor Walk 1 & Hurt
    W, H = 112, 144
    for walk_p, is_h, name in [(1, False, "spr_lictor_walk_1"), (0, True, "spr_lictor_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 68, 38
        if is_h:
            cx -= 5
        draw.ellipse([cx - 12, head_y - 12, cx + 12, head_y + 12], fill=PAL_SKIN[2])
        draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 20, ground_y - 6), (cx - 20, ground_y - 6)], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[4])
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 8, torso_y + 32), (cx - 8 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 8, torso_y + 32), (cx + 8 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)
        fasces_x = cx + 24
        draw.line([(fasces_x, 16), (fasces_x, ground_y)], fill=(160, 115, 65, 255), width=8)
        if is_h:
            draw.line([(cx - 10, head_y - 4), (cx + 10, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_pilate_frames():
    W, H = 144, 176
    for walk_p, is_h, name in [(0, False, "spr_pilate_walk_0"), (1, False, "spr_pilate_walk_1"), (0, True, "spr_pilate_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 78, 42
        if is_h:
            cx -= 6
        draw.arc([cx - 18, head_y - 18, cx + 18, head_y + 8], 160, 380, fill=PAL_GOLD[1], width=4)
        draw.ellipse([cx - 14, head_y - 14, cx + 14, head_y + 14], fill=PAL_SKIN[1], outline=PAL_SKIN[5])
        draw.polygon([(cx - 24, torso_y - 10), (cx + 24, torso_y - 10), (cx + 28, ground_y - 6), (cx - 28, ground_y - 6)], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[5])
        draw.polygon([(cx - 22, torso_y - 8), (cx - 10, torso_y - 8), (cx + 26, ground_y - 8), (cx + 14, ground_y - 8)], fill=(115, 20, 110, 255))
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 10, ground_y - 14), (cx - 12 + int(stride), ground_y - 2), 6, 5, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 10, ground_y - 14), (cx + 12 - int(stride), ground_y - 2), 6, 5, PAL_SKIN)
        if is_h:
            draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_prefect_and_advocate_frames():
    # Prefect Walk 0, Walk 1, Hurt
    W, H = 144, 176
    for walk_p, is_h, name in [(0, False, "spr_prefect_walk_0"), (1, False, "spr_prefect_walk_1"), (0, True, "spr_prefect_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 78, 42
        if is_h:
            cx -= 6
        draw.ellipse([cx - 24, head_y - 30, cx + 24, head_y - 12], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
        draw.ellipse([cx - 16, head_y - 16, cx + 16, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
        draw.polygon([(cx - 24, torso_y - 12), (cx + 24, torso_y - 12), (cx + 20, torso_y + 34), (cx - 20, torso_y + 34)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
        draw.polygon([(cx - 22, torso_y - 8), (cx - 48, torso_y + 60), (cx - 18, ground_y - 12), (cx + 22, torso_y - 8)], fill=(115, 20, 110, 255), outline=(45, 8, 42, 255))
        stride = math.sin(walk_p * math.pi / 2.0) * 14.0
        draw_limb_3tone(draw, (cx - 12, torso_y + 34), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw_limb_3tone(draw, (cx + 12, torso_y + 34), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
        draw.line([(cx + 22, torso_y), (cx + 46, torso_y - 24)], fill=PAL_STEEL[0], width=4)
        if is_h:
            draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

    # Advocate Cast & Hurt
    W, H = 128, 160
    for is_c, is_h, name in [(True, False, "spr_advocate_cast"), (False, True, "spr_advocate_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 70, 36
        if is_h:
            cx -= 5
        draw.ellipse([cx - 14, head_y - 14, cx + 14, head_y + 14], fill=PAL_SKIN[2])
        draw.polygon([(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 24, ground_y - 6), (cx - 24, ground_y - 6)], fill=(235, 230, 220, 255), outline=(60, 55, 50, 255))
        scroll_x = cx + 24
        draw.rectangle([scroll_x - 6, torso_y, scroll_x + 6, torso_y + 24], fill=(245, 235, 195, 255), outline=(120, 95, 45, 255))
        if is_h:
            draw.line([(cx - 10, head_y - 4), (cx + 10, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

def create_mission2_extra():
    # Signifer hurt
    W, H = 120, 152
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2 - 5, H - 8
    torso_y, head_y = 70, 38
    draw.polygon([(cx - 18, head_y - 24), (cx + 18, head_y - 24), (cx + 14, head_y + 12), (cx - 12, head_y + 12)], fill=(115, 105, 95, 255))
    draw.rectangle([cx - 16, torso_y - 6, cx + 16, torso_y + 26], fill=PAL_BRONZE[1])
    draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)
    img.save("assets/sprites/spr_signifer_hurt.png", "PNG")
    print("Generated: assets/sprites/spr_signifer_hurt.png")

    # Thorn Sicarii Leap & Hurt
    W, H = 112, 128
    for is_l, is_h, name in [(True, False, "spr_thorn_sicarii_leap"), (False, True, "spr_thorn_sicarii_hurt")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = (50, 22) if is_l else (60, 32)
        if is_h:
            cx -= 5
        draw.polygon([(cx - 16, head_y - 14), (cx + 16, head_y - 14), (cx + 12, head_y + 16), (cx - 12, head_y + 16)], fill=(38, 58, 32, 255))
        draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 16, torso_y + 28), (cx - 16, torso_y + 28)], fill=(48, 68, 38, 255))
        draw.polygon([(cx - 28, torso_y - 12), (cx - 14, torso_y), (cx - 36, torso_y + 8)], fill=(130, 215, 150, 255))
        if is_h:
            draw.line([(cx - 10, head_y - 4), (cx + 10, head_y + 4)], fill=(240, 50, 50, 255), width=3)
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

    # Corrupted Priest Hurt
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2 - 5, H - 8
    torso_y, head_y = 66, 36
    draw.polygon([(cx - 18, head_y - 16), (cx + 18, head_y - 16), (cx + 16, head_y + 22), (cx - 16, head_y + 22)], fill=(42, 28, 55, 255))
    draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 22, ground_y - 4), (cx - 22, ground_y - 4)], fill=(32, 22, 44, 255))
    draw.line([(cx - 10, head_y - 4), (cx + 10, head_y + 4)], fill=(240, 50, 50, 255), width=3)
    img.save("assets/sprites/spr_corrupted_priest_hurt.png", "PNG")
    print("Generated: assets/sprites/spr_corrupted_priest_hurt.png")

def create_jesus_basket_sprites():
    # spr_jesus_basket_aim, spr_jesus_basket_throw
    W, H = 112, 144
    for is_th, name in [(False, "spr_jesus_basket_aim"), (True, "spr_jesus_basket_throw")]:
        img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        cx, ground_y = W // 2, H - 8
        torso_y, head_y = 68, 38
        # Halo
        draw.ellipse([cx - 26, head_y - 30, cx + 26, head_y + 22], fill=(255, 220, 80, 80))
        draw.ellipse([cx - 22, head_y - 26, cx + 22, head_y + 18], fill=PAL_GOLD[0], outline=PAL_GOLD[4], width=2)
        # Face & Robe
        draw.ellipse([cx - 12, head_y - 14, cx + 12, head_y + 12], fill=PAL_SKIN[1])
        draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 20, ground_y - 6), (cx - 20, ground_y - 6)], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[4])
        draw.polygon([(cx - 14, torso_y - 4), (cx - 4, torso_y - 6), (cx + 18, ground_y - 10), (cx + 8, ground_y - 8)], fill=PAL_CRIMSON[2])
        # Wicker Basket in hands
        bx = cx + 22 if not is_th else cx - 28
        by = torso_y + 12 if not is_th else torso_y - 8
        draw.polygon([(bx - 12, by), (bx + 12, by), (bx + 8, by + 16), (bx - 8, by + 16)], fill=(175, 120, 60, 255), outline=(75, 45, 15, 255), width=2)
        # Bread & Fish in basket
        draw.ellipse([bx - 6, by - 4, bx, by + 4], fill=(215, 155, 80, 255))
        draw.polygon([(bx, by), (bx + 8, by - 4), (bx + 4, by + 4)], fill=(120, 185, 220, 255))
        img.save(f"assets/sprites/{name}.png", "PNG")
        print(f"Generated: assets/sprites/{name}.png")

create_centurion_frames()
create_temple_boss_frames()
create_tribunal_boss_frames()
create_clone_boss_frames()
create_barabbas_frames()
create_caiaphas_frames()
create_judas_frames()
create_praetorian_and_lictor_extra()
create_pilate_frames()
create_prefect_and_advocate_frames()
create_mission2_extra()
create_jesus_basket_sprites()

print("All extra boss frames generated successfully!")

