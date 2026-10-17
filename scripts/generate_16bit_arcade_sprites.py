import os
import math
from PIL import Image, ImageDraw, ImageFilter

os.makedirs("assets/sprites", exist_ok=True)

# 16-bit Master Palette definitions
PAL_WHITE_ROBE = [
    (255, 255, 255, 255), # Specular gleam
    (245, 242, 235, 255), # Linen body
    (210, 205, 195, 255), # Soft fold shade
    (165, 158, 150, 255), # Deep fold shade
    (110, 102,  98, 255), # Cavity crease
    ( 40,  35,  40, 255), # Ink outline
]

PAL_CRIMSON = [
    (255, 150, 160, 255), # Specular rim
    (235,  45,  60, 255), # Vivid scarlet
    (175,  25,  35, 255), # Judean crimson body
    (115,  15,  22, 255), # Deep velvet shade
    ( 65,   8,  14, 255), # Shadow crease
    ( 25,   5,   8, 255), # Ink outline
]

PAL_GOLD = [
    (255, 252, 200, 255), # Specular highlight
    (255, 215,  70, 255), # Radiant gold
    (205, 155,  35, 255), # Burnished gold
    (145,  95,  20, 255), # Bronze shade
    ( 75,  45,  10, 255), # Deep bronze
    ( 25,  18,   5, 255), # Outline
]

PAL_SKIN = [
    (255, 235, 210, 255), # Specular forehead/cheek
    (240, 195, 155, 255), # Judean olive base
    (205, 150, 110, 255), # Soft contour shade
    (160, 105,  70, 255), # Deep shadow
    (105,  60,  35, 255), # Crease
    ( 45,  25,  15, 255), # Outline
]

PAL_HAIR = [
    (185, 125,  70, 255), # Golden auburn strand
    (125,  75,  40, 255), # Chestnut midtone
    ( 75,  40,  20, 255), # Deep brown
    ( 40,  20,  10, 255), # Core shadow
]

PAL_STEEL = [
    (255, 255, 255, 255), # Specular glint
    (225, 230, 240, 255), # Polished steel
    (165, 175, 195, 255), # Steel midtone
    (105, 115, 135, 255), # Steel shade
    ( 55,  60,  75, 255), # Dark metal
    ( 20,  22,  28, 255), # Outline
]

PAL_BRONZE = [
    (255, 240, 180, 255),
    (220, 170,  70, 255),
    (170, 115,  40, 255),
    (110,  68,  22, 255),
    ( 55,  32,  10, 255),
]

def draw_limb_3tone(draw, p1, p2, r1, r2, pal, outline_col=(20, 18, 25, 255)):
    # 1. Dark outline
    draw.line([p1, p2], fill=outline_col, width=int((r1 + r2) + 4))
    draw.ellipse([p1[0]-r1-2, p1[1]-r1-2, p1[0]+r1+2, p1[1]+r1+2], fill=outline_col)
    draw.ellipse([p2[0]-r2-2, p2[1]-r2-2, p2[0]+r2+2, p2[1]+r2+2], fill=outline_col)

    # 2. Deep shade
    draw.line([p1, p2], fill=pal[3], width=int(r1 + r2))
    draw.ellipse([p1[0]-r1, p1[1]-r1, p1[0]+r1, p1[1]+r1], fill=pal[3])
    draw.ellipse([p2[0]-r2, p2[1]-r2, p2[0]+r2, p2[1]+r2], fill=pal[3])

    # 3. Midtone body
    draw.line([p1, p2], fill=pal[1], width=int((r1 + r2) * 0.7))
    draw.ellipse([p1[0]-r1*0.7, p1[1]-r1*0.7, p1[0]+r1*0.7, p1[1]+r1*0.7], fill=pal[1])
    draw.ellipse([p2[0]-r2*0.7, p2[1]-r2*0.7, p2[0]+r2*0.7, p2[1]+r2*0.7], fill=pal[1])

    # 4. Specular highlight line on top ridge
    hp1 = (p1[0], p1[1] - r1 * 0.35)
    hp2 = (p2[0], p2[1] - r2 * 0.35)
    draw.line([hp1, hp2], fill=pal[0], width=max(1, int((r1 + r2) * 0.25)))

def create_jesus_sprite(frame_name, walk_phase=0, is_crouch=False, is_jump=False, is_attack=False, is_melee=False, is_hurt=False):
    # 4x resolution: 112 x 144
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = W // 2
    ground_y = H - 8
    torso_y = 68 if not is_crouch else 88
    head_y = 38 if not is_crouch else 58

    # --- 1. Radiant Golden Halo ---
    halo_r = 26
    for r in range(halo_r, 0, -2):
        alpha = int(45 * (r / halo_r) + 90 * (1.0 - r / halo_r))
        col = (255, 220, 80, alpha)
        draw.ellipse([cx - r, head_y - r - 4, cx + r, head_y + r - 4], fill=col)
    draw.ellipse([cx - halo_r, head_y - halo_r - 4, cx + halo_r, head_y + halo_r - 4], outline=(255, 245, 140, 200), width=2)
    # Gilded Cross within halo
    draw.line([(cx, head_y - halo_r - 2), (cx, head_y + halo_r - 6)], fill=(255, 250, 180, 180), width=2)
    draw.line([(cx - halo_r + 4, head_y - 4), (cx + halo_r - 4, head_y - 4)], fill=(255, 250, 180, 180), width=2)

    # --- 2. Head & Hair ---
    # Chestnut Locks (Behind)
    draw.ellipse([cx - 18, head_y - 12, cx + 18, head_y + 24], fill=PAL_HAIR[2])
    draw.ellipse([cx - 16, head_y - 10, cx + 16, head_y + 22], fill=PAL_HAIR[1])

    # Face Skin
    draw.ellipse([cx - 12, head_y - 14, cx + 12, head_y + 12], fill=PAL_SKIN[1], outline=PAL_SKIN[5], width=1)
    # Cheeks & Nose Highlight
    draw.ellipse([cx - 8, head_y - 10, cx + 8, head_y + 4], fill=PAL_SKIN[0])
    # Eyes & Eyebrows
    draw.line([(cx + 1, head_y - 6), (cx + 7, head_y - 6)], fill=PAL_HAIR[3], width=2)
    draw.point((cx + 4, head_y - 3), fill=(30, 25, 20, 255))
    draw.point((cx + 5, head_y - 4), fill=(255, 255, 255, 255)) # Eye glint
    # Trimmed Beard & Mustache
    draw.polygon([(cx - 10, head_y + 2), (cx + 10, head_y + 2), (cx + 6, head_y + 16), (cx - 6, head_y + 16)], fill=PAL_HAIR[2])
    draw.polygon([(cx - 7, head_y + 4), (cx + 7, head_y + 4), (cx + 4, head_y + 14), (cx - 4, head_y + 14)], fill=PAL_HAIR[1])
    # Flowing locks (Foreground drape)
    draw.arc([cx - 14, head_y - 14, cx + 14, head_y + 26], start=30, end=150, fill=PAL_HAIR[0], width=3)

    # --- 3. Body & Seamless Linen Robes ---
    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0 if not is_crouch and not is_jump else 0.0
    bob = abs(math.sin(walk_phase * math.pi)) * 3.0 if not is_crouch else 0.0

    hip_y = torso_y + 24 - int(bob)
    feet_y = ground_y

    if not is_crouch:
        # Legs & Judean Sandals (Muscular Calves)
        leg1_x = cx - 10 + int(stride)
        leg2_x = cx + 10 - int(stride)
        # Left Leg
        draw_limb_3tone(draw, (cx - 8, hip_y), (leg1_x, feet_y - 6), 7, 5, PAL_SKIN)
        # Right Leg
        draw_limb_3tone(draw, (cx + 8, hip_y), (leg2_x, feet_y - 6), 7, 5, PAL_SKIN)

        # Leather Strapped Sandals
        for ly, lx in [((feet_y - 6), leg1_x), ((feet_y - 6), leg2_x)]:
            draw.rectangle([lx - 7, ly - 6, lx + 7, ly + 2], fill=PAL_HAIR[2])
            draw.line([(lx - 8, ly + 2), (lx + 8, ly + 2)], fill=(30, 20, 15, 255), width=2)
            # Brass buckle
            draw.point((lx, ly - 2), fill=PAL_GOLD[1])

        # Flowing White Robe (Tunic)
        robe_top = torso_y - 6 - int(bob)
        robe_bottom = hip_y + 18
        draw.polygon([
            (cx - 14, robe_top),
            (cx + 14, robe_top),
            (cx + 20, robe_bottom),
            (cx - 20, robe_bottom)
        ], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[5])

        # Vertical Cloth Folds
        for fx in range(-14, 15, 6):
            draw.line([(cx + fx, robe_top + 4), (cx + int(fx * 1.3), robe_bottom - 2)], fill=PAL_WHITE_ROBE[2], width=2)
            draw.line([(cx + fx + 1, robe_top + 6), (cx + int(fx * 1.3) + 1, robe_bottom - 4)], fill=PAL_WHITE_ROBE[0], width=1)

        # Royal Scarlet Sash / Stole draping diagonally from right shoulder to left hip
        draw.polygon([
            (cx - 14, robe_top + 2),
            (cx - 4, robe_top),
            (cx + 18, robe_bottom - 4),
            (cx + 8, robe_bottom - 2)
        ], fill=PAL_CRIMSON[2], outline=PAL_CRIMSON[5])
        # Gold braided border on sash
        draw.line([(cx - 14, robe_top + 2), (cx + 8, robe_bottom - 2)], fill=PAL_GOLD[1], width=2)
        draw.line([(cx - 4, robe_top), (cx + 18, robe_bottom - 4)], fill=PAL_GOLD[1], width=2)
    else:
        # Bunched crouching robes
        draw.ellipse([cx - 24, torso_y - 4, cx + 24, ground_y + 2], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[5])
        draw.arc([cx - 20, torso_y + 6, cx + 20, ground_y - 2], 0, 180, fill=PAL_CRIMSON[2], width=8)
        draw.line([(cx - 22, ground_y), (cx + 22, ground_y)], fill=(30, 20, 15, 255), width=3)

    # --- 4. Arms & Actions ---
    sh_l = (cx - 14, torso_y - 2)
    sh_r = (cx + 14, torso_y - 2)

    if is_attack or is_melee:
        # Thrusting arm with Scourge of Cords / Staff
        reach_x = cx + 38
        reach_y = torso_y + 4
        draw_limb_3tone(draw, sh_r, (cx + 24, torso_y + 2), 6, 5, PAL_WHITE_ROBE)
        draw_limb_3tone(draw, (cx + 24, torso_y + 2), (reach_x, reach_y), 5, 4, PAL_SKIN)
        # Golden Scourge / Whip
        draw.line([(reach_x, reach_y), (reach_x + 18, reach_y - 12)], fill=PAL_GOLD[1], width=3)
        draw.line([(reach_x + 18, reach_y - 12), (reach_x + 28, reach_y + 10)], fill=PAL_GOLD[0], width=2)
        draw.line([(reach_x + 28, reach_y + 10), (reach_x + 40, reach_y - 4)], fill=PAL_GOLD[1], width=2)
        # Spiked morningstar head
        draw.ellipse([reach_x + 38, reach_y - 8, reach_x + 46, reach_y], fill=PAL_GOLD[0], outline=PAL_GOLD[4])
    else:
        # Natural relaxed swinging arms
        arm_swing = math.sin(walk_phase * math.pi / 2.0) * 8.0 if not is_crouch else 0.0
        draw_limb_3tone(draw, sh_l, (sh_l[0] - 8 - int(arm_swing), sh_l[1] + 18), 5, 4, PAL_WHITE_ROBE)
        draw_limb_3tone(draw, (sh_l[0] - 8 - int(arm_swing), sh_l[1] + 18), (sh_l[0] - 4 - int(arm_swing), sh_l[1] + 32), 4, 3, PAL_SKIN)

        draw_limb_3tone(draw, sh_r, (sh_r[0] + 8 + int(arm_swing), sh_r[1] + 18), 5, 4, PAL_WHITE_ROBE)
        draw_limb_3tone(draw, (sh_r[0] + 8 + int(arm_swing), sh_r[1] + 18), (sh_r[0] + 6 + int(arm_swing), sh_r[1] + 32), 4, 3, PAL_SKIN)

    # Save sprite
    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_roman_guard(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = W // 2
    ground_y = H - 8
    torso_y = 66
    head_y = 36

    # Cassis Helmet & Scarlet Crest
    # Crest plume
    draw.polygon([(cx - 16, head_y - 24), (cx + 16, head_y - 24), (cx + 12, head_y - 12), (cx - 12, head_y - 12)], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
    for px in range(-14, 15, 4):
        draw.line([(cx + px, head_y - 24), (cx + px, head_y - 12)], fill=PAL_CRIMSON[0], width=1)

    # Bronze Helmet Dome & Cheek Guards
    draw.ellipse([cx - 13, head_y - 14, cx + 13, head_y + 8], fill=PAL_BRONZE[1], outline=PAL_BRONZE[4], width=2)
    draw.arc([cx - 11, head_y - 12, cx + 11, head_y + 4], 0, 180, fill=PAL_BRONZE[0], width=2)
    # Cheek flaps & neck guard
    draw.rectangle([cx - 14, head_y - 2, cx - 8, head_y + 12], fill=PAL_BRONZE[2], outline=PAL_BRONZE[4])
    draw.rectangle([cx + 8, head_y - 2, cx + 14, head_y + 12], fill=PAL_BRONZE[2], outline=PAL_BRONZE[4])
    # Face slit
    draw.rectangle([cx - 7, head_y - 2, cx + 7, head_y + 8], fill=PAL_SKIN[2])
    draw.point((cx - 3, head_y + 2), fill=(20, 15, 10, 255))
    draw.point((cx + 3, head_y + 2), fill=(20, 15, 10, 255))

    # Lorica Segmentata (Tiered Iron Plate Armor)
    draw.rectangle([cx - 16, torso_y - 8, cx + 16, torso_y + 24], fill=PAL_STEEL[2], outline=PAL_STEEL[5])
    # Horizontal curved steel bands
    for sy in range(torso_y - 6, torso_y + 24, 6):
        draw.line([(cx - 15, sy), (cx + 15, sy)], fill=PAL_STEEL[0], width=2)
        draw.line([(cx - 15, sy + 3), (cx + 15, sy + 3)], fill=PAL_STEEL[4], width=1)

    # Scarlet Tunic Pteruges (Leather & linen hanging strips)
    for py in range(torso_y + 24, torso_y + 36, 4):
        draw.line([(cx - 14, py), (cx + 14, py)], fill=PAL_CRIMSON[2], width=2)

    # Legs & Caligae (Military Hobnailed Boots)
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 28), (cx - 8 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 28), (cx + 8 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)

    # Scutum Shield (Right Hand) - Scarlet curved rectangle with Golden Winged Thunderbolts
    sh_x = cx + 22
    draw.rectangle([sh_x - 12, torso_y - 12, sh_x + 12, torso_y + 36], fill=PAL_CRIMSON[2], outline=PAL_STEEL[1], width=2)
    # Golden Umbo (Center Boss)
    draw.ellipse([sh_x - 6, torso_y + 6, sh_x + 6, torso_y + 18], fill=PAL_GOLD[1], outline=PAL_GOLD[4])
    draw.line([(sh_x - 10, torso_y + 12), (sh_x + 10, torso_y + 12)], fill=PAL_GOLD[0], width=2)
    draw.line([(sh_x, torso_y - 8), (sh_x, torso_y + 32)], fill=PAL_GOLD[0], width=2)

    # Gladius Sword (Left Hand)
    if is_attack:
        draw.line([(cx - 16, torso_y), (cx - 42, torso_y - 8)], fill=PAL_STEEL[0], width=4)
        draw.line([(cx - 16, torso_y + 1), (cx - 42, torso_y - 7)], fill=PAL_STEEL[1], width=2)
        draw.ellipse([cx - 45, torso_y - 10, cx - 41, torso_y - 6], fill=PAL_STEEL[0])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_centurion_brutus(frame_name, is_whip_prepare=False, is_whip_crack=False, is_whip_slam=False, is_hurt=False):
    # Massive 144 x 176 Boss
    W, H = 144, 176
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = W // 2
    ground_y = H - 8
    torso_y = 78
    head_y = 42

    # Transverse Imperial Crest (Side-to-side transverse red horsehair crest)
    draw.ellipse([cx - 28, head_y - 32, cx + 28, head_y - 14], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
    draw.ellipse([cx - 24, head_y - 30, cx + 24, head_y - 16], fill=PAL_CRIMSON[0])

    # Gilded Centurion Helmet with Laurel Wreath
    draw.ellipse([cx - 16, head_y - 16, cx + 16, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
    draw.arc([cx - 14, head_y - 14, cx + 14, head_y + 6], 0, 180, fill=PAL_GOLD[0], width=2)

    # Muscular Lorica Musculata Cuirass (Anatomical golden breastplate)
    draw.polygon([
        (cx - 24, torso_y - 12),
        (cx + 24, torso_y - 12),
        (cx + 20, torso_y + 34),
        (cx - 20, torso_y + 34)
    ], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)

    # Pectoral & Abdominal Muscles embossed in gold
    draw.arc([cx - 18, torso_y - 4, cx - 2, torso_y + 12], 0, 180, fill=PAL_GOLD[0], width=2)
    draw.arc([cx + 2, torso_y - 4, cx + 18, torso_y + 12], 0, 180, fill=PAL_GOLD[0], width=2)
    draw.line([(cx, torso_y + 12), (cx, torso_y + 30)], fill=PAL_GOLD[4], width=2)

    # Imperial Scarlet Cloak (Paludamentum) Billowing Behind
    draw.polygon([
        (cx - 22, torso_y - 8),
        (cx - 48, torso_y + 60),
        (cx - 18, ground_y - 12),
        (cx + 22, torso_y - 8)
    ], fill=PAL_CRIMSON[2], outline=PAL_CRIMSON[5])

    # Massive Muscular Legs & Greaves
    draw_limb_3tone(draw, (cx - 12, torso_y + 34), (cx - 14, ground_y - 4), 9, 7, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 12, torso_y + 34), (cx + 14, ground_y - 4), 9, 7, PAL_BRONZE)

    # Multi-tailed Flagrum Scourge (Scorpion Whip)
    if is_whip_crack or is_whip_slam:
        whip_tip_x = cx - 58
        whip_tip_y = torso_y + 14
        draw_limb_3tone(draw, (cx - 20, torso_y), (cx - 36, torso_y - 12), 8, 6, PAL_SKIN)
        draw_limb_3tone(draw, (cx - 36, torso_y - 12), (cx - 48, torso_y), 7, 5, PAL_SKIN)
        # 3 Lead-weighted whip tails
        for t in range(-6, 7, 6):
            draw.line([(cx - 48, torso_y), (whip_tip_x, whip_tip_y + t * 4)], fill=PAL_STEEL[1], width=3)
            # Lead knuckle weights
            draw.ellipse([whip_tip_x - 4, whip_tip_y + t * 4 - 4, whip_tip_x + 4, whip_tip_y + t * 4 + 4], fill=PAL_STEEL[0], outline=PAL_STEEL[4])
    else:
        # Resting whip in hand
        draw_limb_3tone(draw, (cx - 20, torso_y), (cx - 28, torso_y + 24), 8, 6, PAL_SKIN)
        draw.line([(cx - 28, torso_y + 24), (cx - 32, ground_y - 10)], fill=PAL_STEEL[2], width=3)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_judas_sprite(frame_name, is_toss=False, is_dash=False, is_stab=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = W // 2
    ground_y = H - 8
    torso_y = 68
    head_y = 38

    # Dishonored Hood & Cowl (Dark Olive / Ochre)
    draw.polygon([(cx - 18, head_y - 18), (cx + 18, head_y - 18), (cx + 14, head_y + 18), (cx - 14, head_y + 18)], fill=(65, 75, 45, 255), outline=(25, 30, 18, 255))
    # Face in shadow
    draw.ellipse([cx - 10, head_y - 10, cx + 10, head_y + 10], fill=PAL_SKIN[2])
    draw.point((cx - 3, head_y - 2), fill=(20, 20, 20, 255))
    draw.point((cx + 3, head_y - 2), fill=(20, 20, 20, 255))
    draw.arc([cx - 6, head_y + 4, cx + 6, head_y + 10], 180, 360, fill=(40, 20, 15, 255), width=2) # Sneer

    # Dark Ochre Tunic & Mantle
    draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 22, torso_y + 36), (cx - 22, torso_y + 36)], fill=(85, 95, 55, 255), outline=(30, 35, 20, 255))

    # Legs & sandals
    draw_limb_3tone(draw, (cx - 8, torso_y + 32), (cx - 10, ground_y - 4), 6, 5, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 32), (cx + 10, ground_y - 4), 6, 5, PAL_SKIN)

    # Leather Pouch with 30 Pieces of Silver
    pouch_x = cx + 16
    draw.ellipse([pouch_x - 8, torso_y + 18, pouch_x + 8, torso_y + 32], fill=(120, 75, 40, 255), outline=(50, 30, 15, 255))
    # Spilling silver shekels
    for dx, dy in [(-2, -4), (3, -6), (6, -2)]:
        draw.ellipse([pouch_x + dx - 3, torso_y + 18 + dy - 3, pouch_x + dx + 3, torso_y + 18 + dy + 3], fill=PAL_STEEL[0], outline=PAL_STEEL[3])

    if is_toss:
        # Arm flung forward throwing silver coins
        draw_limb_3tone(draw, (cx - 14, torso_y), (cx - 34, torso_y - 8), 6, 4, PAL_SKIN)
        for i in range(4):
            sx = cx - 42 - i * 10
            sy = torso_y - 12 + i * 4
            draw.ellipse([sx - 4, sy - 4, sx + 4, sy + 4], fill=PAL_STEEL[0], outline=PAL_STEEL[3])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_pilate_sprite(frame_name, is_basin=False, is_gavel=False):
    W, H = 144, 176
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = W // 2
    ground_y = H - 8
    torso_y = 78
    head_y = 42

    # Golden Laurel Wreath
    draw.arc([cx - 18, head_y - 18, cx + 18, head_y + 8], 160, 380, fill=PAL_GOLD[1], width=4)
    for lx in range(-14, 15, 6):
        draw.polygon([(cx + lx, head_y - 16), (cx + lx + 4, head_y - 20), (cx + lx + 2, head_y - 14)], fill=PAL_GOLD[0])

    # Aristocratic Roman Face with silvered hair
    draw.ellipse([cx - 14, head_y - 14, cx + 14, head_y + 14], fill=PAL_SKIN[1], outline=PAL_SKIN[5])
    draw.ellipse([cx - 10, head_y - 10, cx + 10, head_y + 4], fill=PAL_SKIN[0])
    draw.line([(cx - 16, head_y - 10), (cx - 12, head_y + 8)], fill=(180, 185, 195, 255), width=3) # Silver temples
    draw.line([(cx + 16, head_y - 10), (cx + 12, head_y + 8)], fill=(180, 185, 195, 255), width=3)

    # Roman Toga Praetexta (Broad Tyrian Purple Border)
    draw.polygon([
        (cx - 24, torso_y - 10),
        (cx + 24, torso_y - 10),
        (cx + 28, ground_y - 6),
        (cx - 28, ground_y - 6)
    ], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[5])

    # Tyrian Purple Border Swag
    draw.polygon([
        (cx - 22, torso_y - 8),
        (cx - 10, torso_y - 8),
        (cx + 26, ground_y - 8),
        (cx + 14, ground_y - 8)
    ], fill=(115, 20, 110, 255), outline=(50, 8, 45, 255))
    draw.line([(cx - 22, torso_y - 8), (cx + 14, ground_y - 8)], fill=PAL_GOLD[1], width=2)
    draw.line([(cx - 10, torso_y - 8), (cx + 26, ground_y - 8)], fill=PAL_GOLD[1], width=2)

    # Ceremonial Bronze Water Basin
    if is_basin:
        bx = cx - 36
        by = torso_y + 20
        # Basin Stand
        draw.polygon([(bx - 18, by), (bx + 18, by), (bx + 12, by + 26), (bx - 12, by + 26)], fill=PAL_BRONZE[1], outline=PAL_BRONZE[4])
        # Water & Droplets
        draw.ellipse([bx - 14, by - 6, bx + 14, by + 6], fill=(120, 210, 255, 220), outline=(255, 255, 255, 240))
        # Hands washing in basin
        draw_limb_3tone(draw, (cx - 14, torso_y), (bx - 2, by), 6, 4, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 14, torso_y), (bx + 4, by), 6, 4, PAL_SKIN)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_roman_archer(frame_name, is_fire=False, is_duck=False, is_hurt=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx = W // 2
    ground_y = H - 8
    torso_y = 68 if not is_duck else 88
    head_y = 38 if not is_duck else 58

    # Leather helmet with bronze plate
    draw.ellipse([cx - 12, head_y - 12, cx + 12, head_y + 8], fill=(120, 80, 45, 255), outline=PAL_HAIR[3])
    draw.arc([cx - 10, head_y - 10, cx + 10, head_y + 4], 0, 180, fill=PAL_BRONZE[1], width=2)
    draw.rectangle([cx - 6, head_y - 2, cx + 6, head_y + 6], fill=PAL_SKIN[1])

    # Leather Scale Armor & Tunic
    draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 16, torso_y + 28), (cx - 16, torso_y + 28)], fill=(145, 95, 55, 255), outline=(50, 30, 15, 255))
    # Scales
    for y in range(torso_y - 2, torso_y + 24, 5):
        draw.line([(cx - 12, y), (cx + 12, y)], fill=(185, 130, 75, 255), width=2)

    # Legs & Boots
    if not is_duck:
        draw_limb_3tone(draw, (cx - 8, torso_y + 28), (cx - 8, ground_y - 4), 5, 4, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 8, torso_y + 28), (cx + 8, ground_y - 4), 5, 4, PAL_SKIN)
    else:
        draw.ellipse([cx - 18, torso_y + 12, cx + 18, ground_y], fill=(90, 55, 30, 255))

    # Composite Bow (Recurve)
    bow_x = cx + 22
    draw.arc([bow_x - 16, torso_y - 18, bow_x + 16, torso_y + 36], 270, 90, fill=(160, 105, 40, 255), width=3)
    draw.line([(bow_x, torso_y - 18), (bow_x, torso_y + 36)], fill=(230, 230, 240, 220), width=1) # String
    if is_fire:
        # Drawn arrow
        draw.line([(bow_x - 22, torso_y + 9), (bow_x + 12, torso_y + 9)], fill=(220, 220, 230, 255), width=2)
        draw.polygon([(bow_x + 12, torso_y + 7), (bow_x + 18, torso_y + 9), (bow_x + 12, torso_y + 11)], fill=PAL_STEEL[0])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_roman_heavy(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 128, 152
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx = W // 2
    ground_y = H - 8
    torso_y = 70
    head_y = 38

    # Heavy Bronze Galea with Black Horsehair Crest
    draw.ellipse([cx - 20, head_y - 28, cx + 20, head_y - 12], fill=(30, 25, 35, 255), outline=(10, 8, 12, 255))
    draw.ellipse([cx - 15, head_y - 14, cx + 15, head_y + 10], fill=PAL_BRONZE[1], outline=PAL_BRONZE[4], width=2)

    # Muscular Cuirass
    draw.polygon([(cx - 22, torso_y - 10), (cx + 22, torso_y - 10), (cx + 18, torso_y + 32), (cx - 18, torso_y + 32)], fill=PAL_BRONZE[2], outline=PAL_BRONZE[4], width=2)
    draw.arc([cx - 16, torso_y - 2, cx - 2, torso_y + 14], 0, 180, fill=PAL_BRONZE[0], width=2)
    draw.arc([cx + 2, torso_y - 2, cx + 16, torso_y + 14], 0, 180, fill=PAL_BRONZE[0], width=2)

    # Massive Greaves & Boots
    stride = math.sin(walk_phase * math.pi / 2.0) * 10.0
    draw_limb_3tone(draw, (cx - 10, torso_y + 32), (cx - 10 + int(stride), ground_y - 4), 8, 6, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 10, torso_y + 32), (cx + 10 - int(stride), ground_y - 4), 8, 6, PAL_BRONZE)

    # Pilum (Heavy Spear)
    draw.line([(cx - 24, torso_y - 40), (cx - 24, ground_y + 4)], fill=(120, 80, 40, 255), width=4)
    # Long iron barbed shank
    draw.line([(cx - 24, torso_y - 40), (cx - 24, torso_y - 68)], fill=PAL_STEEL[1], width=3)
    draw.polygon([(cx - 27, torso_y - 68), (cx - 21, torso_y - 68), (cx - 24, torso_y - 78)], fill=PAL_STEEL[0])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_sicarii(frame_name, is_slash=False, is_throw=False, is_cling=False):
    W, H = 112, 128
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx = W // 2
    ground_y = H - 8
    torso_y = 60
    head_y = 32

    # Hooded Midnight Cowl
    draw.polygon([(cx - 16, head_y - 14), (cx + 16, head_y - 14), (cx + 12, head_y + 16), (cx - 12, head_y + 16)], fill=(32, 28, 48, 255), outline=(12, 10, 20, 255))
    # Glowing vigilant eyes in cowl shadow
    draw.ellipse([cx - 7, head_y, cx - 3, head_y + 4], fill=(255, 230, 100, 255))
    draw.ellipse([cx + 3, head_y, cx + 7, head_y + 4], fill=(255, 230, 100, 255))

    # Agile Shadow Tunic
    draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 16, torso_y + 28), (cx - 16, torso_y + 28)], fill=(45, 40, 62, 255), outline=(18, 15, 26, 255))

    # Leather Vambraces & Boots
    draw_limb_3tone(draw, (cx - 8, torso_y + 28), (cx - 8, ground_y - 4), 6, 4, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 28), (cx + 8, ground_y - 4), 6, 4, PAL_SKIN)

    # Curved Sica Daggers (Steel blades with gold hilt)
    draw.arc([cx - 28, torso_y - 8, cx - 12, torso_y + 20], 90, 270, fill=PAL_STEEL[0], width=3)
    draw.arc([cx + 12, torso_y - 8, cx + 28, torso_y + 20], 270, 90, fill=PAL_STEEL[0], width=3)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_projectiles_and_items():
    # 1. Spikeless Arrow (16x8 -> 64x32)
    img_arrow = Image.new("RGBA", (64, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_arrow)
    d.line([(4, 16), (56, 16)], fill=(140, 90, 45, 255), width=3)
    d.polygon([(52, 11), (62, 16), (52, 21)], fill=PAL_STEEL[0], outline=PAL_STEEL[4])
    d.polygon([(4, 11), (14, 16), (4, 21)], fill=PAL_CRIMSON[1])
    img_arrow.save("assets/sprites/spr_arrow.png", "PNG")

    # 2. Silver Coin (8x8 -> 32x32)
    img_coin = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_coin)
    d.ellipse([3, 3, 29, 29], fill=PAL_STEEL[1], outline=PAL_STEEL[4], width=2)
    d.ellipse([6, 6, 26, 26], fill=PAL_STEEL[0])
    d.line([(16, 8), (16, 24)], fill=PAL_STEEL[3], width=2)
    img_coin.save("assets/sprites/spr_silver_coin_projectile.png", "PNG")

    # 3. Loaf of Bread (40x32)
    img_bread = Image.new("RGBA", (40, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_bread)
    d.ellipse([4, 6, 36, 26], fill=(215, 155, 80, 255), outline=(110, 65, 25, 255), width=2)
    d.arc([8, 8, 32, 20], 180, 360, fill=(255, 210, 140, 255), width=3)
    img_bread.save("assets/sprites/spr_bread_loaf.png", "PNG")

    # 4. Fish Projectile (48x32)
    img_fish = Image.new("RGBA", (48, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_fish)
    d.polygon([(6, 16), (16, 8), (34, 11), (42, 16), (34, 21), (16, 24)], fill=(120, 185, 220, 255), outline=(40, 85, 120, 255), width=2)
    d.polygon([(6, 16), (0, 8), (0, 24)], fill=(80, 140, 180, 255))
    d.point((36, 13), fill=(20, 30, 40, 255))
    img_fish.save("assets/sprites/spr_fish_projectile.png", "PNG")

    # 5. UI Heart Pip (40x40)
    img_heart = Image.new("RGBA", (40, 40), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_heart)
    d.polygon([(20, 36), (4, 18), (8, 6), (18, 8), (20, 12), (22, 8), (32, 6), (36, 18)], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5], width=2)
    d.ellipse([8, 8, 16, 16], fill=(255, 180, 190, 255)) # highlight
    img_heart.save("assets/sprites/spr_icon_heart.png", "PNG")

    # 6. UI Wrath Icon (48x48)
    img_wrath = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_wrath)
    d.ellipse([4, 4, 44, 44], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=3)
    d.ellipse([10, 10, 38, 38], fill=(60, 15, 20, 255))
    # Golden Lion Silhouette
    d.ellipse([18, 16, 30, 28], fill=PAL_GOLD[0])
    img_wrath.save("assets/sprites/spr_icon_wrath.png", "PNG")

    # 7. Wicker Basket (88x72)
    img_bsk = Image.new("RGBA", (88, 72), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_bsk)
    d.polygon([(14, 20), (74, 20), (64, 66), (24, 66)], fill=(175, 120, 60, 255), outline=(75, 45, 15, 255), width=3)
    for x in range(20, 70, 8):
        d.line([(x, 22), (x, 64)], fill=(125, 80, 35, 255), width=2)
    for y in range(26, 62, 8):
        d.line([(18, y), (70, y)], fill=(225, 175, 100, 255), width=2)
    img_bsk.save("assets/sprites/spr_basket_loaves_fishes.png", "PNG")
    print("Generated 16-bit projectiles and items!")

def create_temple_boss(frame_name, walk_phase=0, is_slash=False, is_slam=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 82, 46

    # Gilded High-Priest Guard Helmet with Temple Menorah Emblem
    draw.ellipse([cx - 20, head_y - 24, cx + 20, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[5], width=2)
    draw.line([(cx, head_y - 20), (cx, head_y - 4)], fill=PAL_GOLD[0], width=3)

    # Face & Beard
    draw.ellipse([cx - 14, head_y - 10, cx + 14, head_y + 12], fill=PAL_SKIN[2])
    draw.polygon([(cx - 12, head_y + 6), (cx + 12, head_y + 6), (cx, head_y + 24)], fill=PAL_HAIR[2])

    # Burnished Golden Temple Cuirass & Tyrian Purple Sash
    draw.polygon([(cx - 26, torso_y - 12), (cx + 26, torso_y - 12), (cx + 22, torso_y + 38), (cx - 22, torso_y + 38)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
    draw.arc([cx - 20, torso_y - 4, cx - 2, torso_y + 14], 0, 180, fill=PAL_GOLD[0], width=2)
    draw.arc([cx + 2, torso_y - 4, cx + 20, torso_y + 14], 0, 180, fill=PAL_GOLD[0], width=2)
    # Purple sash
    draw.polygon([(cx - 24, torso_y + 24), (cx + 24, torso_y + 28), (cx + 20, torso_y + 38), (cx - 20, torso_y + 34)], fill=(120, 25, 110, 255))

    # Muscular Greaved Legs
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 14, torso_y + 38), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 14, torso_y + 38), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)

    # Great Temple Mace (Heavy Spiked Golden Morningstar)
    if is_slash or is_slam:
        mace_x = cx - 48
        mace_y = torso_y - 10
        draw.line([(cx - 22, torso_y), (mace_x, mace_y)], fill=(140, 95, 45, 255), width=6)
        draw.ellipse([mace_x - 14, mace_y - 14, mace_x + 14, mace_y + 14], fill=PAL_GOLD[0], outline=PAL_GOLD[4], width=2)
        # Spikes
        for angle in range(0, 360, 45):
            rad = math.radians(angle)
            sx = mace_x + math.cos(rad) * 20
            sy = mace_y + math.sin(rad) * 20
            draw.line([(mace_x, mace_y), (sx, sy)], fill=PAL_GOLD[0], width=3)
    else:
        draw.line([(cx - 22, torso_y), (cx - 36, ground_y - 10)], fill=(140, 95, 45, 255), width=5)
        draw.ellipse([cx - 44, ground_y - 24, cx - 28, ground_y - 4], fill=PAL_GOLD[0], outline=PAL_GOLD[4])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_tribunal_boss(frame_name, walk_phase=0, is_slash=False, is_slam=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 82, 46

    # Gilded Crest & Corinthian Praetor Helmet
    draw.polygon([(cx - 24, head_y - 32), (cx + 24, head_y - 32), (cx, head_y - 16)], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
    draw.ellipse([cx - 18, head_y - 18, cx + 18, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)

    # Imperial Muscle Armor with Lion-head Pauldrons
    draw.polygon([(cx - 26, torso_y - 12), (cx + 26, torso_y - 12), (cx + 22, torso_y + 38), (cx - 22, torso_y + 38)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
    # Lion pauldrons
    draw.ellipse([cx - 34, torso_y - 16, cx - 18, torso_y + 4], fill=PAL_GOLD[0], outline=PAL_GOLD[4])
    draw.ellipse([cx + 18, torso_y - 16, cx + 34, torso_y + 4], fill=PAL_GOLD[0], outline=PAL_GOLD[4])

    # Flowing Scarlet Cape
    draw.polygon([(cx - 26, torso_y - 8), (cx - 56, torso_y + 64), (cx - 24, ground_y - 10), (cx + 26, torso_y - 8)], fill=PAL_CRIMSON[2], outline=PAL_CRIMSON[5])

    # Legs & Greaves
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 14, torso_y + 38), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 14, torso_y + 38), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)

    # Holy Lance (Pilum of Longinus)
    draw.line([(cx + 24, torso_y - 50), (cx + 24, ground_y)], fill=(120, 75, 35, 255), width=5)
    draw.polygon([(cx + 19, torso_y - 50), (cx + 29, torso_y - 50), (cx + 24, torso_y - 74)], fill=PAL_STEEL[0], outline=PAL_STEEL[4])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_tormentor(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 68, 38

    # Scarred Face & Hood
    draw.polygon([(cx - 14, head_y - 16), (cx + 14, head_y - 16), (cx + 12, head_y + 14), (cx - 12, head_y + 14)], fill=(65, 45, 35, 255), outline=(30, 20, 15, 255))
    draw.ellipse([cx - 10, head_y - 8, cx + 10, head_y + 8], fill=PAL_SKIN[3])
    draw.line([(cx - 6, head_y - 4), (cx + 4, head_y + 6)], fill=(180, 40, 40, 255), width=2) # Battle scar

    # Executioner's Heavy Leather Vest
    draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 18, torso_y + 32), (cx - 18, torso_y + 32)], fill=(75, 45, 30, 255), outline=(35, 20, 10, 255), width=2)
    # Iron studs
    for y in range(torso_y, torso_y + 28, 8):
        draw.point((cx - 8, y), fill=PAL_STEEL[1])
        draw.point((cx + 8, y), fill=PAL_STEEL[1])

    # Legs & Heavy Boots
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 32), (cx - 8 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 32), (cx + 8 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)

    # Left Hand: Flaming Pitch Torch (Glowing dynamic fire)
    torch_x, torch_y = cx + 26, torso_y - 10
    draw.line([(cx + 16, torso_y), (torch_x, torch_y + 18)], fill=(110, 70, 35, 255), width=4)
    # Flame corona
    for r, col in [(16, (255, 90, 20, 90)), (11, (255, 170, 30, 160)), (6, (255, 245, 120, 230))]:
        draw.ellipse([torch_x - r, torch_y - r, torch_x + r, torch_y + r], fill=col)

    # Right Hand: Spiked Cudgel
    cudgel_x = cx - 26 if not is_attack else cx - 44
    draw.line([(cx - 16, torso_y), (cudgel_x, torso_y - 12)], fill=(80, 50, 25, 255), width=6)
    draw.ellipse([cudgel_x - 8, torso_y - 20, cudgel_x + 8, torso_y - 4], fill=(45, 48, 55, 255), outline=PAL_STEEL[1], width=2)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_thorn_sicarii(frame_name, is_crouch=False, is_slash=False):
    W, H = 112, 128
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = (60, 32) if not is_crouch else (80, 52)

    # Camouflaged Forest Vine & Ivy Cowl
    draw.polygon([(cx - 16, head_y - 14), (cx + 16, head_y - 14), (cx + 12, head_y + 16), (cx - 12, head_y + 16)], fill=(38, 58, 32, 255), outline=(15, 28, 12, 255))
    # Glowing predatory emerald eyes
    draw.ellipse([cx - 7, head_y, cx - 3, head_y + 4], fill=(120, 255, 140, 255))
    draw.ellipse([cx + 3, head_y, cx + 7, head_y + 4], fill=(120, 255, 140, 255))

    # Tunic with thorny bramble wraps
    draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 16, torso_y + 28), (cx - 16, torso_y + 28)], fill=(48, 68, 38, 255), outline=(20, 32, 16, 255))
    draw.line([(cx - 14, torso_y), (cx + 14, torso_y + 20)], fill=(125, 95, 45, 255), width=2) # Vine

    # Legs & Boots
    draw_limb_3tone(draw, (cx - 8, torso_y + 28), (cx - 8, ground_y - 4), 6, 4, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 28), (cx + 8, ground_y - 4), 6, 4, PAL_SKIN)

    # Serrated Poison Thorn Blades
    draw.polygon([(cx - 28, torso_y - 12), (cx - 14, torso_y), (cx - 36, torso_y + 8)], fill=(130, 215, 150, 255), outline=(30, 75, 40, 255))
    draw.polygon([(cx + 28, torso_y - 12), (cx + 14, torso_y), (cx + 36, torso_y + 8)], fill=(130, 215, 150, 255), outline=(30, 75, 40, 255))

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_corrupted_priest(frame_name, is_cast=False, is_hurt=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 66, 36

    # Dark Ritualistic Talit / Hood (Charcoal with Tattered Purple Ribbons)
    draw.polygon([(cx - 18, head_y - 16), (cx + 18, head_y - 16), (cx + 16, head_y + 22), (cx - 16, head_y + 22)], fill=(42, 28, 55, 255), outline=(18, 10, 26, 255))
    # Pallid, corrupted skin & glowing dark void eyes
    draw.ellipse([cx - 10, head_y - 8, cx + 10, head_y + 10], fill=(190, 180, 195, 255))
    draw.ellipse([cx - 7, head_y - 2, cx - 3, head_y + 2], fill=(220, 40, 255, 255))
    draw.ellipse([cx + 3, head_y - 2, cx + 7, head_y + 2], fill=(220, 40, 255, 255))

    # Long Dark Robes with Tattered Hem
    draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 22, ground_y - 4), (cx - 22, ground_y - 4)], fill=(32, 22, 44, 255), outline=(15, 8, 22, 255))
    # Tattered hem cuts
    for x in range(cx - 20, cx + 20, 6):
        draw.line([(x, ground_y - 10), (x + 3, ground_y - 4)], fill=(160, 50, 200, 255), width=2)

    # Channeling Dark Occult Orbs
    orb_x = cx - 30 if is_cast else cx - 18
    orb_y = torso_y - 8
    draw_limb_3tone(draw, (cx - 14, torso_y), (orb_x + 8, orb_y + 12), 5, 4, (180, 175, 190, 255))
    # Violet Dark Energy Pulsing Orb
    for r, col in [(18, (160, 30, 220, 70)), (12, (200, 60, 255, 160)), (6, (255, 220, 255, 240))]:
        draw.ellipse([orb_x - r, orb_y - r, orb_x + r, orb_y + r], fill=col)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_clone_boss(frame_name, walk_phase=0, is_channel=False, is_attack=False, is_hurt=False):
    # Shadow Inverted Jesus (Christus Tenebris)
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 68, 38

    # Unholy Dark Violet Halo with crackling lightning
    halo_r = 26
    for r in range(halo_r, 0, -2):
        alpha = int(50 * (r / halo_r) + 110 * (1.0 - r / halo_r))
        draw.ellipse([cx - r, head_y - r - 4, cx + r, head_y + r - 4], fill=(140, 20, 220, alpha))
    draw.ellipse([cx - halo_r, head_y - halo_r - 4, cx + halo_r, head_y + halo_r - 4], outline=(240, 80, 255, 220), width=2)

    # Pale Ashen Face & Dark Hair
    draw.ellipse([cx - 18, head_y - 12, cx + 18, head_y + 24], fill=(28, 22, 32, 255))
    draw.ellipse([cx - 12, head_y - 14, cx + 12, head_y + 12], fill=(200, 195, 210, 255))
    # Glowing Crimson Demon Eyes
    draw.ellipse([cx - 7, head_y - 3, cx - 2, head_y + 2], fill=(255, 30, 40, 255))
    draw.ellipse([cx + 2, head_y - 3, cx + 7, head_y + 2], fill=(255, 30, 40, 255))

    # Obsidian / Charcoal Tunic with Deep Wine Crimson Stole
    draw.polygon([(cx - 14, torso_y - 6), (cx + 14, torso_y - 6), (cx + 20, ground_y - 12), (cx - 20, ground_y - 12)], fill=(30, 28, 38, 255), outline=(12, 10, 18, 255))
    draw.polygon([(cx - 14, torso_y - 4), (cx - 4, torso_y - 6), (cx + 18, ground_y - 16), (cx + 8, ground_y - 14)], fill=(125, 12, 28, 255))

    # Legs & Black Leather Sandals
    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 24), (cx - 8 + int(stride), ground_y - 4), 7, 5, (190, 185, 200, 255))
    draw_limb_3tone(draw, (cx + 8, torso_y + 24), (cx + 8 - int(stride), ground_y - 4), 7, 5, (190, 185, 200, 255))

    # Corrupted Scourge Whip
    draw_limb_3tone(draw, (cx + 14, torso_y), (cx + 34, torso_y + 6), 6, 4, (190, 185, 200, 255))
    draw.line([(cx + 34, torso_y + 6), (cx + 52, torso_y - 10)], fill=(160, 30, 220, 255), width=3)
    draw.ellipse([cx + 50, torso_y - 14, cx + 58, torso_y - 6], fill=(255, 40, 60, 255))

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_barabbas_boss(frame_name, walk_phase=0, is_slash=False, is_slam=False, is_hurt=False):
    # Massive 152 x 176 Rebel Brawler
    W, H = 152, 176
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 76, 40

    # Wild Unkempt Mane & Fierce Beard
    draw.ellipse([cx - 24, head_y - 20, cx + 24, head_y + 26], fill=(45, 28, 16, 255), outline=(20, 12, 8, 255))
    draw.ellipse([cx - 14, head_y - 10, cx + 14, head_y + 12], fill=PAL_SKIN[3])
    # War paint & ferocious scowl
    draw.line([(cx - 8, head_y - 2), (cx + 8, head_y - 2)], fill=(20, 15, 10, 255), width=3)
    draw.polygon([(cx - 16, head_y + 8), (cx + 16, head_y + 8), (cx, head_y + 28)], fill=(45, 28, 16, 255))

    # Muscular Bare Torso with Leather Harness & Scars
    draw.polygon([(cx - 26, torso_y - 10), (cx + 26, torso_y - 10), (cx + 20, torso_y + 36), (cx - 20, torso_y + 36)], fill=PAL_SKIN[2], outline=PAL_SKIN[5], width=2)
    # Heavy chest muscles & harness
    draw.arc([cx - 20, torso_y - 2, cx - 2, torso_y + 16], 0, 180, fill=PAL_SKIN[1], width=2)
    draw.arc([cx + 2, torso_y - 2, cx + 20, torso_y + 16], 0, 180, fill=PAL_SKIN[1], width=2)
    draw.line([(cx - 22, torso_y - 8), (cx + 18, torso_y + 34)], fill=(75, 45, 25, 255), width=4)

    # Broken Iron Manacles with Dangling Chains on Wrists!
    arm_l = (cx - 40, torso_y + 8) if not is_slash else (cx - 52, torso_y - 16)
    arm_r = (cx + 40, torso_y + 8) if not is_slash else (cx + 52, torso_y - 16)
    draw_limb_3tone(draw, (cx - 22, torso_y), arm_l, 9, 7, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 22, torso_y), arm_r, 9, 7, PAL_SKIN)
    # Heavy iron wrist cuffs
    draw.rectangle([arm_l[0] - 6, arm_l[1] - 6, arm_l[0] + 6, arm_l[1] + 6], fill=PAL_STEEL[3], outline=PAL_STEEL[5])
    draw.rectangle([arm_r[0] - 6, arm_r[1] - 6, arm_r[0] + 6, arm_r[1] + 6], fill=PAL_STEEL[3], outline=PAL_STEEL[5])
    # Dangling broken chain links swinging from cuffs
    for i in range(3):
        draw.ellipse([arm_l[0] - 3, arm_l[1] + 8 + i * 6, arm_l[0] + 3, arm_l[1] + 14 + i * 6], outline=PAL_STEEL[1], width=2)
        draw.ellipse([arm_r[0] - 3, arm_r[1] + 8 + i * 6, arm_r[0] + 3, arm_r[1] + 14 + i * 6], outline=PAL_STEEL[1], width=2)

    # Dual Brutal Falchion Cleavers
    draw.polygon([(arm_l[0] - 2, arm_l[1] - 4), (arm_l[0] - 16, arm_l[1] - 38), (arm_l[0] - 32, arm_l[1] - 28), (arm_l[0] - 6, arm_l[1] + 2)], fill=PAL_STEEL[0], outline=PAL_STEEL[4], width=2)
    draw.polygon([(arm_r[0] + 2, arm_r[1] - 4), (arm_r[0] + 16, arm_r[1] - 38), (arm_r[0] + 32, arm_r[1] - 28), (arm_r[0] + 6, arm_r[1] + 2)], fill=PAL_STEEL[0], outline=PAL_STEEL[4], width=2)

    # Massive Legs & Wrapped Sandals
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 12, torso_y + 36), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 12, torso_y + 36), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_SKIN)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_caiaphas_boss(frame_name, is_cast=False, is_hurt=False):
    W, H = 128, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 70, 36

    # High Priest Golden Miter (Headdress) with Golden Crown Plate ("Holiness unto the Lord")
    draw.polygon([(cx - 18, head_y - 24), (cx + 18, head_y - 24), (cx + 14, head_y + 8), (cx - 14, head_y + 8)], fill=(240, 245, 255, 255), outline=PAL_GOLD[4], width=2)
    draw.rectangle([cx - 16, head_y - 14, cx + 16, head_y - 6], fill=PAL_GOLD[0], outline=PAL_GOLD[4])

    # Robe of the Ephod (Celestial Blue with Gold Fringe)
    draw.polygon([(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 24, ground_y - 6), (cx - 24, ground_y - 6)], fill=(32, 85, 160, 255), outline=(12, 35, 75, 255), width=2)

    # Sacred Hoshen Breastplate with 12 Radiant Gemstones!
    bp_x, bp_y = cx - 14, torso_y - 2
    draw.rectangle([bp_x, bp_y, bp_x + 28, bp_y + 24], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)
    gem_cols = [
        (255, 60, 60), (255, 220, 60), (60, 255, 120), (60, 200, 255),
        (220, 60, 255), (255, 140, 40), (40, 240, 200), (180, 255, 80),
        (255, 180, 220), (120, 60, 255), (255, 240, 180), (180, 140, 90)
    ]
    for idx, col in enumerate(gem_cols):
        gx = bp_x + 3 + (idx % 4) * 6
        gy = bp_y + 3 + (idx // 4) * 7
        draw.rectangle([gx, gy, gx + 4, gy + 4], fill=col)

    # Arms raised calling down judgment lightning
    if is_cast:
        draw_limb_3tone(draw, (cx - 18, torso_y), (cx - 36, torso_y - 28), 6, 4, PAL_SKIN)
        draw_limb_3tone(draw, (cx + 18, torso_y), (cx + 36, torso_y - 28), 6, 4, PAL_SKIN)
        # Sparkles on fingertips
        draw.ellipse([cx - 40, torso_y - 32, cx - 32, torso_y - 24], fill=(255, 255, 200, 255))
        draw.ellipse([cx + 32, torso_y - 32, cx + 40, torso_y - 24], fill=(255, 255, 200, 255))

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_signifer(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 120, 152
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 70, 38

    # Wolf Pelt Draped over Cassis Helmet (Wolf fangs framing face, ears on top)
    draw.polygon([(cx - 18, head_y - 24), (cx + 18, head_y - 24), (cx + 14, head_y + 12), (cx - 14, head_y + 12)], fill=(115, 105, 95, 255), outline=(45, 40, 35, 255))
    # Wolf ears
    draw.polygon([(cx - 16, head_y - 24), (cx - 10, head_y - 34), (cx - 4, head_y - 24)], fill=(135, 125, 115, 255))
    draw.polygon([(cx + 4, head_y - 24), (cx + 10, head_y - 34), (cx + 16, head_y - 24)], fill=(135, 125, 115, 255))
    # Face slit
    draw.rectangle([cx - 8, head_y - 4, cx + 8, head_y + 8], fill=PAL_SKIN[2])

    # Lorica Squamata (Scale Armor)
    draw.rectangle([cx - 16, torso_y - 6, cx + 16, torso_y + 26], fill=PAL_BRONZE[1], outline=PAL_BRONZE[4])

    # Legs & Caligae
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 26), (cx - 8 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 26), (cx + 8 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)

    # Towering Signum Standard (Golden Hand & Phalerae Medallions)
    pole_x = cx + 26
    draw.line([(pole_x, 10), (pole_x, ground_y)], fill=(130, 85, 40, 255), width=5)
    # Golden Hand (Manus) at apex
    draw.ellipse([pole_x - 7, 8, pole_x + 7, 24], fill=PAL_GOLD[0], outline=PAL_GOLD[4])
    # 4 Silver Phalerae Disks along shaft
    for dy in range(32, 80, 14):
        draw.ellipse([pole_x - 9, dy, pole_x + 9, dy + 10], fill=PAL_STEEL[0], outline=PAL_STEEL[3])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_praetorian(frame_name, walk_phase=0, is_bash=False, is_hurt=False):
    W, H = 128, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 74, 40

    # Imperial Black & Gold Attic Helmet with Regal Purple Plume
    draw.ellipse([cx - 20, head_y - 30, cx + 20, head_y - 14], fill=(125, 20, 120, 255), outline=(50, 8, 48, 255))
    draw.ellipse([cx - 15, head_y - 16, cx + 15, head_y + 10], fill=(35, 32, 42, 255), outline=PAL_GOLD[1], width=2)

    # Black Polished Lorica Segmentata with Gilded Joint Hinges
    draw.rectangle([cx - 18, torso_y - 8, cx + 18, torso_y + 28], fill=(32, 30, 38, 255), outline=PAL_GOLD[2], width=2)
    for y in range(torso_y - 4, torso_y + 24, 7):
        draw.line([(cx - 16, y), (cx + 16, y)], fill=PAL_GOLD[1], width=2)

    # Legs & Greaves
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 10, torso_y + 28), (cx - 10 + int(stride), ground_y - 4), 7, 5, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 10, torso_y + 28), (cx + 10 - int(stride), ground_y - 4), 7, 5, PAL_BRONZE)

    # Heavy Oval Praetorian Shield with Golden Imperial Scorpion
    sh_x = cx + 26 if not is_bash else cx - 24
    draw.ellipse([sh_x - 14, torso_y - 14, sh_x + 14, torso_y + 38], fill=(28, 25, 36, 255), outline=PAL_GOLD[1], width=3)
    # Golden Scorpion Emblazoned on Shield
    draw.circle((sh_x, torso_y + 8), 5, fill=PAL_GOLD[0])
    draw.arc([sh_x - 8, torso_y + 6, sh_x + 8, torso_y + 20], 0, 180, fill=PAL_GOLD[0], width=2)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_lictor(frame_name, walk_phase=0, is_swing=False, is_hurt=False):
    W, H = 112, 144
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 68, 38

    # Roman Toga draped over scarlet tunic
    draw.ellipse([cx - 12, head_y - 12, cx + 12, head_y + 12], fill=PAL_SKIN[2])
    draw.polygon([(cx - 16, torso_y - 6), (cx + 16, torso_y - 6), (cx + 20, ground_y - 6), (cx - 20, ground_y - 6)], fill=PAL_WHITE_ROBE[1], outline=PAL_WHITE_ROBE[4])

    # Legs & Sandals
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 8, torso_y + 32), (cx - 8 + int(stride), ground_y - 4), 6, 5, PAL_SKIN)
    draw_limb_3tone(draw, (cx + 8, torso_y + 32), (cx + 8 - int(stride), ground_y - 4), 6, 5, PAL_SKIN)

    # Roman Fasces (Bundle of birch rods bound with scarlet thongs, enclosing bronze axe)
    fasces_x = cx + 24 if not is_swing else cx - 36
    draw.line([(fasces_x, 16), (fasces_x, ground_y)], fill=(160, 115, 65, 255), width=8)
    for y in range(24, ground_y - 8, 14):
        draw.line([(fasces_x - 5, y), (fasces_x + 5, y)], fill=PAL_CRIMSON[1], width=2) # Red straps
    # Bronze executioner axe blade emerging from fasces bundle
    draw.polygon([(fasces_x + 4, 22), (fasces_x + 18, 14), (fasces_x + 22, 34), (fasces_x + 4, 30)], fill=PAL_BRONZE[0], outline=PAL_BRONZE[4])

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_prefect(frame_name, walk_phase=0, is_slash=False, is_rush=False, is_hurt=False):
    W, H = 144, 176
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 78, 42

    # Prefect Equestrian Crested Galea
    draw.ellipse([cx - 24, head_y - 30, cx + 24, head_y - 12], fill=PAL_CRIMSON[1], outline=PAL_CRIMSON[5])
    draw.ellipse([cx - 16, head_y - 16, cx + 16, head_y + 12], fill=PAL_GOLD[1], outline=PAL_GOLD[4], width=2)

    # Gilded Musculata Cuirass with Silver Gorgon Head
    draw.polygon([(cx - 24, torso_y - 12), (cx + 24, torso_y - 12), (cx + 20, torso_y + 34), (cx - 20, torso_y + 34)], fill=PAL_GOLD[2], outline=PAL_GOLD[5], width=2)
    draw.ellipse([cx - 8, torso_y, cx + 8, torso_y + 16], fill=PAL_STEEL[0]) # Silver medallion

    # Regal Purple Cape
    draw.polygon([(cx - 22, torso_y - 8), (cx - 48, torso_y + 60), (cx - 18, ground_y - 12), (cx + 22, torso_y - 8)], fill=(115, 20, 110, 255), outline=(45, 8, 42, 255))

    # Legs & Greaves
    stride = math.sin(walk_phase * math.pi / 2.0) * 12.0
    draw_limb_3tone(draw, (cx - 12, torso_y + 34), (cx - 14 + int(stride), ground_y - 4), 9, 7, PAL_BRONZE)
    draw_limb_3tone(draw, (cx + 12, torso_y + 34), (cx + 14 - int(stride), ground_y - 4), 9, 7, PAL_BRONZE)

    # Spatha Heavy Sword
    draw.line([(cx + 22, torso_y), (cx + 46, torso_y - 24)], fill=PAL_STEEL[0], width=4)

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

def create_advocate(frame_name, is_shout=False, is_hurt=False):
    W, H = 128, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 8
    torso_y, head_y = 70, 36

    # Aristocratic Roman Prosecutor in Patrician Toga with Gold Fibula
    draw.ellipse([cx - 14, head_y - 14, cx + 14, head_y + 14], fill=PAL_SKIN[2])
    draw.polygon([(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 24, ground_y - 6), (cx - 24, ground_y - 6)], fill=(235, 230, 220, 255), outline=(60, 55, 50, 255))
    draw.ellipse([cx - 12, torso_y - 6, cx - 4, torso_y + 2], fill=PAL_GOLD[0]) # Gold fibula brooch

    # Holding Indictment Papyrus Scroll
    scroll_x = cx + 24
    draw.rectangle([scroll_x - 6, torso_y, scroll_x + 6, torso_y + 24], fill=(245, 235, 195, 255), outline=(120, 95, 45, 255))

    out_path = f"assets/sprites/{frame_name}.png"
    img.save(out_path, "PNG")
    print(f"Generated: {out_path} ({W}x{H})")

# Generate Mission 2, 3, and 4 Bosses and Enemies
create_temple_boss("spr_temple_boss_idle")
create_temple_boss("spr_temple_boss_slash", is_slash=True)
create_temple_boss("spr_temple_boss_slam", is_slam=True)

create_tribunal_boss("spr_tribunal_idle")
create_tribunal_boss("spr_tribunal_slash", is_slash=True)
create_tribunal_boss("spr_tribunal_slam", is_slam=True)

create_tormentor("spr_tormentor_walk_0", walk_phase=0)
create_tormentor("spr_tormentor_walk_1", walk_phase=1)
create_tormentor("spr_tormentor_attack", is_attack=True)
create_tormentor("spr_tormentor_hurt", is_hurt=True)

create_thorn_sicarii("spr_thorn_sicarii_crouch", is_crouch=True)
create_thorn_sicarii("spr_thorn_sicarii_slash", is_slash=True)

create_corrupted_priest("spr_corrupted_priest_stand")
create_corrupted_priest("spr_corrupted_priest_cast", is_cast=True)

create_clone_boss("spr_clone_idle")
create_clone_boss("spr_clone_channel", is_channel=True)

create_barabbas_boss("spr_barabbas_idle")
create_barabbas_boss("spr_barabbas_slash", is_slash=True)
create_barabbas_boss("spr_barabbas_slam", is_slam=True)

create_caiaphas_boss("spr_caiaphas_stand")
create_caiaphas_boss("spr_caiaphas_cast", is_cast=True)

create_signifer("spr_signifer_walk_0", walk_phase=0)
create_signifer("spr_signifer_walk_1", walk_phase=1)
create_signifer("spr_signifer_attack", is_attack=True)

create_praetorian("spr_praetorian_walk_0", walk_phase=0)
create_praetorian("spr_praetorian_shield_bash", is_bash=True)

create_lictor("spr_lictor_walk_0", walk_phase=0)
create_lictor("spr_lictor_fasces_swing", is_swing=True)

create_prefect("spr_prefect_idle")
create_prefect("spr_prefect_slash", is_slash=True)

create_advocate("spr_advocate_idle")
create_advocate("spr_advocate_shout", is_shout=True)

print("All 16-Bit Arcade Enemies Generation Complete!")

