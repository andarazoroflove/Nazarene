import os
import math
from PIL import Image, ImageDraw

os.makedirs("assets/sprites", exist_ok=True)

# =========================================================================
# CAPCOM CPS-2 / NEO GEO 16-BIT MASTER PALETTES
# =========================================================================
C_INK = (16, 12, 20, 255) # Heavy dark contour ink

# 1. Olive Judean Flesh (4-5 stops)
PAL_SKIN = [
    (255, 240, 222, 255), # 0: Specular cheek/forehead
    (245, 195, 150, 255), # 1: Warm base flesh
    (198, 134,  88, 255), # 2: Muscle contour
    (148,  82,  52, 255), # 3: Deep abdominal/pec crease
    ( 95,  46,  28, 255), # 4: Core cavity shadow
]

# 2. Roman / Bronze Tanned Flesh
PAL_SKIN_BRONZE = [
    (255, 230, 200, 255),
    (230, 170, 120, 255),
    (175, 110,  65, 255),
    (125,  68,  38, 255),
    ( 78,  38,  20, 255),
]

# 3. Fighter Tape / Hand Wraps & Linen
PAL_WRAP = [
    (255, 255, 255, 255), # Specular highlight
    (238, 234, 224, 255), # Clean linen
    (188, 180, 168, 255), # Midtone wrap
    (132, 124, 114, 255), # Crease shadow
    ( 72,  66,  60, 255), # Tape weave shadow
]

# 4. Sacred Judean Crimson / Roman Scarlet
PAL_CRIMSON = [
    (255, 155, 170, 255), # Rim glint
    (235,  45,  60, 255), # Pure scarlet
    (172,  22,  34, 255), # Deep crimson body
    (110,  14,  22, 255), # Shadow velvet
    ( 52,   8,  12, 255), # Crease
]

# 5. Burnished Imperial Gold
PAL_GOLD = [
    (255, 254, 210, 255), # Specular flash
    (255, 218,  72, 255), # Bright gold
    (204, 156,  34, 255), # Burnished midtone
    (142,  94,  18, 255), # Bronze shade
    ( 72,  44,   8, 255), # Deep contour
]

# 6. Polished Steel
PAL_STEEL = [
    (255, 255, 255, 255), # Glint
    (226, 232, 242, 255), # Polished surface
    (162, 174, 196, 255), # Steel midtone
    (102, 114, 136, 255), # Form shadow
    ( 52,  58,  74, 255), # Dark metal cavity
]

# 7. Heavy Bronze
PAL_BRONZE = [
    (255, 242, 185, 255),
    (224, 174,  72, 255),
    (168, 116,  42, 255),
    (108,  66,  20, 255),
    ( 54,  30,   8, 255),
]

# 8. Biblical Auburn Hair & Beard
PAL_AUBURN = [
    (198, 138,  78, 255), # Golden auburn strand
    (142,  86,  46, 255), # Midtone chestnut
    ( 88,  46,  22, 255), # Deep brown
    ( 44,  20,  10, 255), # Ink root
]

# 9. Denim Blue / Celestial Cobalt
PAL_COBALT = [
    (150, 210, 255, 255),
    ( 55, 130, 230, 255),
    ( 28,  75, 175, 255),
    ( 15,  40, 110, 255),
    (  8,  20,  65, 255),
]

# =========================================================================
# CAPCOM DRAWING PRIMITIVES
# =========================================================================
def draw_ink_poly(draw, points, fill_col, outline_col=C_INK, outline_width=2):
    draw.polygon(points, fill=fill_col, outline=outline_col, width=outline_width)

def draw_ink_circle(draw, bbox, fill_col, outline_col=C_INK, outline_width=2):
    draw.ellipse(bbox, fill=fill_col, outline=outline_col, width=outline_width)

def draw_fighter_wraps(draw, p1, p2, r1, r2, pal=PAL_WRAP):
    # 1. Dark ink backing
    draw.line([p1, p2], fill=C_INK, width=int(r1 + r2 + 4))
    draw.ellipse([p1[0]-r1-2, p1[1]-r1-2, p1[0]+r1+2, p1[1]+r1+2], fill=C_INK)
    draw.ellipse([p2[0]-r2-2, p2[1]-r2-2, p2[0]+r2+2, p2[1]+r2+2], fill=C_INK)
    # 2. Base wrap body
    draw.line([p1, p2], fill=pal[2], width=int(r1 + r2))
    draw.ellipse([p1[0]-r1, p1[1]-r1, p1[0]+r1, p1[1]+r1], fill=pal[2])
    draw.ellipse([p2[0]-r2, p2[1]-r2, p2[0]+r2, p2[1]+r2], fill=pal[2])
    # 3. Highlight strip
    draw.line([(p1[0]-1, p1[1]-2), (p2[0]-1, p2[1]-2)], fill=pal[1], width=int((r1+r2)*0.6))
    # 4. Cross-hatching tape bands
    steps = 6
    for i in range(steps):
        t = i / float(steps)
        tx = int(p1[0] + (p2[0] - p1[0]) * t)
        ty = int(p1[1] + (p2[1] - p1[1]) * t)
        draw.line([(tx - 6, ty - 3), (tx + 6, ty + 3)], fill=pal[3], width=2)
        draw.line([(tx - 5, ty - 4), (tx + 5, ty + 2)], fill=pal[0], width=1)

def draw_capcom_muscle_limb(draw, p1, p2, r1, r2, pal=PAL_SKIN):
    # 1. Ink outline
    draw.line([p1, p2], fill=C_INK, width=int(r1 + r2 + 4))
    draw.ellipse([p1[0]-r1-2, p1[1]-r1-2, p1[0]+r1+2, p1[1]+r1+2], fill=C_INK)
    draw.ellipse([p2[0]-r2-2, p2[1]-r2-2, p2[0]+r2+2, p2[1]+r2+2], fill=C_INK)
    # 2. Deep muscle shadow
    draw.line([p1, p2], fill=pal[3], width=int(r1 + r2))
    draw.ellipse([p1[0]-r1, p1[1]-r1, p1[0]+r1, p1[1]+r1], fill=pal[3])
    draw.ellipse([p2[0]-r2, p2[1]-r2, p2[0]+r2, p2[1]+r2], fill=pal[3])
    # 3. Midtone flesh
    draw.line([p1, p2], fill=pal[1], width=int((r1 + r2) * 0.72))
    draw.ellipse([p1[0]-r1*0.72, p1[1]-r1*0.72, p1[0]+r1*0.72, p1[1]+r1*0.72], fill=pal[1])
    draw.ellipse([p2[0]-r2*0.72, p2[1]-r2*0.72, p2[0]+r2*0.72, p2[1]+r2*0.72], fill=pal[1])
    # 4. Specular ridge
    hp1 = (p1[0], p1[1] - r1 * 0.35)
    hp2 = (p2[0], p2[1] - r2 * 0.35)
    draw.line([hp1, hp2], fill=pal[0], width=max(1, int((r1 + r2) * 0.28)))

# Save wrapper
def save_sprite(img, name):
    out_path = f"assets/sprites/{name}.png"
    img.save(out_path, "PNG")
    print(f"Generated Capcom Arcade Sprite: {out_path} ({img.size[0]}x{img.size[1]})")


# =========================================================================
# PART 2: PLAYER (JESUS - THE NAZARENE)
# =========================================================================
def create_capcom_player(frame_name, walk_phase=0, is_crouch=False, is_jump=False, is_attack=False, is_melee=False, is_hurt=False):
    W, H = 160, 200
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = W // 2
    ground_y = H - 10
    head_y = 52 if not is_crouch else 78
    torso_y = 98 if not is_crouch else 118

    if is_jump:
        head_y -= 20
        torso_y -= 20
        ground_y -= 26
    if is_hurt:
        cx -= 8

    # 1. Divine Radiant Nimbus (CPS-2 Sunburst Halo)
    for r, col in [(42, (255, 215, 60, 50)), (34, (255, 230, 100, 100)), (26, (255, 250, 180, 160))]:
        draw.ellipse([cx - r, head_y - r - 4, cx + r, head_y + r - 4], fill=col)
    draw.ellipse([cx - 28, head_y - 32, cx + 28, head_y + 24], outline=C_INK, width=4)
    draw.ellipse([cx - 27, head_y - 31, cx + 27, head_y + 23], outline=PAL_GOLD[1], width=2)
    draw.ellipse([cx - 26, head_y - 30, cx + 26, head_y + 22], outline=PAL_GOLD[0], width=1)

    # 2. Billowing Mantle (Back Layer)
    mantle_swing = math.sin(walk_phase * math.pi / 2.0) * 16.0 if not is_crouch else 0.0
    if is_jump:
        # Mantle billows upwards
        draw_ink_poly(draw, [(cx - 28, torso_y), (cx - 58, torso_y + 40), (cx - 32, torso_y + 80), (cx - 14, torso_y + 20)], PAL_CRIMSON[2])
        draw_ink_poly(draw, [(cx + 14, torso_y + 20), (cx + 58, torso_y + 40), (cx + 32, torso_y + 80), (cx + 28, torso_y)], PAL_CRIMSON[3])
    else:
        draw_ink_poly(draw, [(cx - 34, head_y - 4), (cx - 54 + int(mantle_swing), torso_y + 36), (cx - 26 + int(mantle_swing), torso_y + 72), (cx - 18, torso_y)], PAL_CRIMSON[2])
        draw_ink_poly(draw, [(cx + 18, head_y - 4), (cx + 50 - int(mantle_swing), torso_y + 32), (cx + 26 - int(mantle_swing), torso_y + 68), (cx + 16, torso_y)], PAL_CRIMSON[3])

    # 3. Back Hair Locks
    draw_ink_poly(draw, [(cx - 22, head_y - 14), (cx - 32, head_y + 24), (cx - 16, head_y + 36), (cx - 12, head_y)], PAL_AUBURN[2])
    draw_ink_poly(draw, [(cx + 12, head_y), (cx + 28, head_y + 24), (cx + 16, head_y + 36), (cx + 18, head_y - 14)], PAL_AUBURN[2])

    # 4. Legs & Stance
    if is_jump:
        # Tucked acrobatic jump legs
        draw_ink_poly(draw, [(cx - 24, torso_y + 36), (cx - 42, torso_y + 60), (cx - 20, torso_y + 60), (cx - 10, torso_y + 36)], PAL_WRAP[2])
        draw_ink_poly(draw, [(cx + 10, torso_y + 36), (cx + 20, torso_y + 60), (cx + 42, torso_y + 60), (cx + 24, torso_y + 36)], PAL_WRAP[3])
        # Tucked boots
        draw_ink_poly(draw, [(cx - 44, torso_y + 60), (cx - 18, torso_y + 60), (cx - 22, torso_y + 74), (cx - 42, torso_y + 74)], (120, 75, 40, 255))
        draw_ink_poly(draw, [(cx + 18, torso_y + 60), (cx + 44, torso_y + 60), (cx + 42, torso_y + 74), (cx + 22, torso_y + 74)], (100, 60, 30, 255))
    elif is_crouch:
        # Low martial crouch
        draw_ink_poly(draw, [(cx - 28, torso_y + 34), (cx - 52, ground_y - 12), (cx - 22, ground_y - 12), (cx - 12, torso_y + 34)], PAL_WRAP[2])
        draw_ink_poly(draw, [(cx + 12, torso_y + 34), (cx + 22, ground_y - 12), (cx + 52, ground_y - 12), (cx + 28, torso_y + 34)], PAL_WRAP[3])
        draw_ink_poly(draw, [(cx - 54, ground_y - 12), (cx - 18, ground_y - 12), (cx - 16, ground_y), (cx - 56, ground_y)], (120, 75, 40, 255))
        draw_ink_poly(draw, [(cx + 18, ground_y - 12), (cx + 54, ground_y - 12), (cx + 56, ground_y), (cx + 16, ground_y)], (100, 60, 30, 255))
    else:
        # Dynamic stride or stance
        stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
        draw_ink_poly(draw, [(cx - 24, torso_y + 44), (cx - 40 + int(stride), ground_y - 20), (cx - 20 + int(stride), ground_y - 20), (cx - 10, torso_y + 44)], PAL_WRAP[2])
        draw_ink_poly(draw, [(cx + 10, torso_y + 44), (cx + 20 - int(stride), ground_y - 20), (cx + 42 - int(stride), ground_y - 20), (cx + 24, torso_y + 44)], PAL_WRAP[3])
        # Pants folds
        draw.line([(cx - 32 + int(stride), ground_y - 30), (cx - 18 + int(stride), ground_y - 32)], fill=PAL_WRAP[1], width=3)
        draw.line([(cx + 18 - int(stride), ground_y - 30), (cx + 34 - int(stride), ground_y - 32)], fill=PAL_WRAP[1], width=3)
        # Combat boots with soles
        draw_ink_poly(draw, [(cx - 44 + int(stride), ground_y - 20), (cx - 16 + int(stride), ground_y - 20), (cx - 14 + int(stride), ground_y), (cx - 48 + int(stride), ground_y)], (120, 75, 40, 255))
        draw_ink_poly(draw, [(cx + 16 - int(stride), ground_y - 20), (cx + 44 - int(stride), ground_y - 20), (cx + 48 - int(stride), ground_y), (cx + 14 - int(stride), ground_y)], (100, 60, 30, 255))
        draw.line([(cx - 48 + int(stride), ground_y - 2), (cx - 14 + int(stride), ground_y - 2)], fill=(45, 25, 12, 255), width=3)
        draw.line([(cx + 14 - int(stride), ground_y - 2), (cx + 48 - int(stride), ground_y - 2)], fill=(45, 25, 12, 255), width=3)

    # 5. Torso - Linen Tunic & Draped Crimson Mantle
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y - 14), (cx + 22, torso_y + 46), (cx - 22, torso_y + 46)], PAL_WRAP[1])
    # Diagonal Crimson Mantle with Gold Trimming across chest
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y + 12), (cx + 22, torso_y + 46), (cx - 16, torso_y + 46)], PAL_CRIMSON[2])
    # Gold fringe trim
    draw.line([(cx - 26, torso_y - 14), (cx + 26, torso_y + 12)], fill=PAL_GOLD[1], width=3)
    draw.line([(cx - 26, torso_y - 14), (cx + 26, torso_y + 12)], fill=PAL_GOLD[0], width=1)
    # Tunic neck opening showing muscular collarbone & chest
    draw_ink_poly(draw, [(cx - 8, torso_y - 14), (cx + 8, torso_y - 14), (cx, torso_y - 2)], PAL_SKIN[2])

    # Thick Braided Belt & Buckle
    draw.rectangle([cx - 23, torso_y + 36, cx + 23, torso_y + 46], fill=(130, 80, 45, 255), outline=C_INK, width=2)
    draw.rectangle([cx - 7, torso_y + 37, cx + 7, torso_y + 45], fill=PAL_GOLD[1], outline=C_INK, width=2)
    draw.rectangle([cx - 4, torso_y + 39, cx + 4, torso_y + 43], fill=PAL_GOLD[0])

    # 6. Muscular Arms & Hand Wraps
    if is_attack or is_melee:
        # Dynamic thrust punch / projectile throw forward!
        arm_l_sh = (cx - 26, torso_y - 8)
        arm_l_el = (cx - 52, torso_y - 4)
        arm_l_wr = (cx - 74, torso_y - 2)
        # Shoulder & Bicep
        draw_ink_circle(draw, [arm_l_sh[0] - 12, arm_l_sh[1] - 8, arm_l_sh[0] + 12, arm_l_sh[1] + 16], PAL_SKIN[1])
        draw_capcom_muscle_limb(draw, arm_l_sh, arm_l_el, 9, 8, PAL_SKIN)
        # Wrapped Forearm & Fist
        draw_fighter_wraps(draw, arm_l_el, arm_l_wr, 8, 7, PAL_WRAP)
        draw_ink_circle(draw, [arm_l_wr[0] - 10, arm_l_wr[1] - 8, arm_l_wr[0] + 10, arm_l_wr[1] + 8], PAL_WRAP[1])
        # Holy projectile spark flash at fist
        for r, col in [(24, (255, 240, 160, 100)), (14, (255, 255, 200, 200)), (6, (255, 255, 255, 255))]:
            draw.ellipse([arm_l_wr[0] - 8 - r, arm_l_wr[1] - r, arm_l_wr[0] - 8 + r, arm_l_wr[1] + r], fill=col)

        # Right arm pulled back in combat guard
        arm_r_sh = (cx + 26, torso_y - 8)
        arm_r_el = (cx + 42, torso_y)
        arm_r_wr = (cx + 36, torso_y + 24)
        draw_ink_circle(draw, [arm_r_sh[0] - 12, arm_r_sh[1] - 8, arm_r_sh[0] + 12, arm_r_sh[1] + 16], PAL_SKIN[2])
        draw_fighter_wraps(draw, arm_r_el, arm_r_wr, 7, 6, PAL_WRAP)
        draw_ink_circle(draw, [arm_r_wr[0] - 8, arm_r_wr[1] - 2, arm_r_wr[0] + 8, arm_r_wr[1] + 14], PAL_WRAP[1])
    else:
        # Idle / Walk ready stance
        arm_l_sh = (cx - 26, torso_y - 8)
        arm_l_el = (cx - 46, torso_y + 12)
        arm_l_wr = (cx - 36, torso_y + 36)
        draw_ink_circle(draw, [arm_l_sh[0] - 12, arm_l_sh[1] - 8, arm_l_sh[0] + 12, arm_l_sh[1] + 16], PAL_SKIN[1])
        draw.arc([arm_l_sh[0] - 10, arm_l_sh[1] - 6, arm_l_sh[0] + 10, arm_l_sh[1] + 10], 0, 180, fill=PAL_SKIN[0], width=2)
        draw_capcom_muscle_limb(draw, arm_l_sh, arm_l_el, 9, 8, PAL_SKIN)
        draw_fighter_wraps(draw, arm_l_el, arm_l_wr, 8, 7, PAL_WRAP)
        draw_ink_circle(draw, [arm_l_wr[0] - 9, arm_l_wr[1] - 2, arm_l_wr[0] + 9, arm_l_wr[1] + 14], PAL_WRAP[1])

        arm_r_sh = (cx + 26, torso_y - 8)
        arm_r_el = (cx + 46, torso_y + 8)
        arm_r_wr = (cx + 42, torso_y + 32)
        draw_ink_circle(draw, [arm_r_sh[0] - 12, arm_r_sh[1] - 8, arm_r_sh[0] + 12, arm_r_sh[1] + 16], PAL_SKIN[2])
        draw_fighter_wraps(draw, arm_r_el, arm_r_wr, 7, 6, PAL_WRAP)
        draw_ink_circle(draw, [arm_r_wr[0] - 8, arm_r_wr[1] - 2, arm_r_wr[0] + 8, arm_r_wr[1] + 14], PAL_WRAP[1])

    # 7. Capcom Heroic Head & Face
    # Neck
    draw_ink_poly(draw, [(cx - 10, head_y + 12), (cx + 10, head_y + 12), (cx + 12, torso_y - 12), (cx - 12, torso_y - 12)], PAL_SKIN[2])
    # Head & Jaw
    draw_ink_poly(draw, [(cx - 14, head_y - 14), (cx + 14, head_y - 14), (cx + 12, head_y + 16), (cx, head_y + 24), (cx - 12, head_y + 16)], PAL_SKIN[1])
    # Beard
    draw_ink_poly(draw, [(cx - 13, head_y + 4), (cx + 13, head_y + 4), (cx + 10, head_y + 22), (cx, head_y + 28), (cx - 10, head_y + 22)], PAL_AUBURN[2])
    draw.polygon([(cx - 8, head_y + 10), (cx + 8, head_y + 10), (cx, head_y + 24)], fill=PAL_AUBURN[1])

    # Grimace / Determined Teeth
    draw.rectangle([cx - 5, head_y + 8, cx + 5, head_y + 13], fill=C_INK)
    draw.rectangle([cx - 4, head_y + 9, cx + 4, head_y + 11], fill=(255, 255, 255, 255))
    # Mustache
    draw.arc([cx - 8, head_y + 4, cx, head_y + 10], 180, 360, fill=PAL_AUBURN[0], width=2)
    draw.arc([cx, head_y + 4, cx + 8, head_y + 10], 180, 360, fill=PAL_AUBURN[0], width=2)

    # Nose Bridge
    draw.line([(cx - 1, head_y - 3), (cx - 1, head_y + 5)], fill=PAL_SKIN[3], width=2)
    draw.point((cx, head_y + 4), fill=PAL_SKIN[0])

    # Eyes & Brow Ridge
    draw.line([(cx - 10, head_y - 5), (cx - 2, head_y - 3)], fill=PAL_AUBURN[3], width=3)
    draw.line([(cx + 2, head_y - 3), (cx + 10, head_y - 5)], fill=PAL_AUBURN[3], width=3)
    draw.rectangle([cx - 8, head_y - 2, cx - 3, head_y + 2], fill=(255, 255, 255, 255), outline=C_INK)
    draw.rectangle([cx + 3, head_y - 2, cx + 8, head_y + 2], fill=(255, 255, 255, 255), outline=C_INK)
    draw.point((cx - 5, head_y), fill=(50, 30, 20, 255))
    draw.point((cx + 6, head_y), fill=(50, 30, 20, 255))

    # Foreground Locks
    draw_ink_poly(draw, [(cx - 14, head_y - 18), (cx + 14, head_y - 18), (cx + 16, head_y - 6), (cx - 16, head_y - 6)], PAL_AUBURN[1])
    draw.polygon([(cx - 14, head_y - 10), (cx - 20, head_y + 8), (cx - 12, head_y + 12)], fill=PAL_AUBURN[0])
    draw.polygon([(cx + 14, head_y - 10), (cx + 20, head_y + 8), (cx + 12, head_y + 12)], fill=PAL_AUBURN[0])

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

# Generate All Player Animations
create_capcom_player("spr_player_idle")
create_capcom_player("spr_player_walk_0", walk_phase=0)
create_capcom_player("spr_player_walk_1", walk_phase=1)
create_capcom_player("spr_player_walk_2", walk_phase=2)
create_capcom_player("spr_player_walk_3", walk_phase=3)
create_capcom_player("spr_player_crouch", is_crouch=True)
create_capcom_player("spr_player_jump", is_jump=True)
create_capcom_player("spr_player_attack_stand", is_attack=True)
create_capcom_player("spr_player_attack_crouch", is_crouch=True, is_attack=True)
create_capcom_player("spr_player_melee_strike", is_melee=True)
create_capcom_player("spr_player_hurt", is_hurt=True)


# =========================================================================
# PART 3: MISSION 1 ENEMIES & BOSSES
# =========================================================================
def create_capcom_roman_guard(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 82, 42
    if is_hurt: cx -= 6

    # 1. Cassis Helmet & Scarlet Horsehair Crest
    draw_ink_circle(draw, [cx - 24, head_y - 30, cx + 24, head_y - 12], PAL_CRIMSON[1])
    for px in range(-20, 21, 4):
        draw.line([(cx + px, head_y - 30), (cx + px, head_y - 14)], fill=PAL_CRIMSON[0], width=2)
    # Bronze dome & cheek flaps
    draw_ink_circle(draw, [cx - 16, head_y - 14, cx + 16, head_y + 12], PAL_BRONZE[1])
    draw.rectangle([cx - 16, head_y, cx - 10, head_y + 16], fill=PAL_BRONZE[2], outline=C_INK)
    draw.rectangle([cx + 10, head_y, cx + 16, head_y + 16], fill=PAL_BRONZE[2], outline=C_INK)
    # Stern soldier face
    draw.rectangle([cx - 9, head_y, cx + 9, head_y + 10], fill=PAL_SKIN[2])
    draw.line([(cx - 7, head_y + 2), (cx + 7, head_y + 2)], fill=C_INK, width=2)

    # 2. Lorica Segmentata (Overlapping Steel Plates)
    draw_ink_poly(draw, [(cx - 22, torso_y - 10), (cx + 22, torso_y - 10), (cx + 18, torso_y + 36), (cx - 18, torso_y + 36)], PAL_STEEL[2])
    for sy in range(torso_y - 6, torso_y + 34, 7):
        draw.line([(cx - 18, sy), (cx + 18, sy)], fill=PAL_STEEL[0], width=3)
        draw.line([(cx - 18, sy + 3), (cx + 18, sy + 3)], fill=PAL_STEEL[4], width=1)
        draw.point((cx - 8, sy + 1), fill=PAL_BRONZE[0]) # Brass rivets
        draw.point((cx + 8, sy + 1), fill=PAL_BRONZE[0])

    # Leather Pteruges Skirt
    for py in range(torso_y + 36, torso_y + 48, 4):
        draw.line([(cx - 16, py), (cx + 16, py)], fill=PAL_CRIMSON[2], width=2)

    # 3. Legs & Caligae
    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 36), (cx - 10 + int(stride), ground_y - 6), 7, 6, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 36), (cx + 10 - int(stride), ground_y - 6), 7, 6, PAL_SKIN)
    draw_ink_poly(draw, [(cx - 18 + int(stride), ground_y - 10), (cx - 4 + int(stride), ground_y - 10), (cx - 4 + int(stride), ground_y), (cx - 20 + int(stride), ground_y)], (90, 55, 30, 255))
    draw_ink_poly(draw, [(cx + 4 - int(stride), ground_y - 10), (cx + 18 - int(stride), ground_y - 10), (cx + 20 - int(stride), ground_y), (cx + 4 - int(stride), ground_y)], (90, 55, 30, 255))

    # 4. Scutum Shield (Heavy Lacquered Red Curved Shield)
    sh_x = cx + 28
    draw_ink_poly(draw, [(sh_x - 14, torso_y - 18), (sh_x + 14, torso_y - 18), (sh_x + 16, torso_y + 44), (sh_x - 16, torso_y + 44)], PAL_CRIMSON[1])
    # Bronze Shield Rim
    draw.polygon([(sh_x - 14, torso_y - 18), (sh_x + 14, torso_y - 18), (sh_x + 16, torso_y + 44), (sh_x - 16, torso_y + 44)], outline=PAL_BRONZE[0], width=3)
    # Golden Winged Thunderbolts
    draw_ink_circle(draw, [sh_x - 7, torso_y + 8, sh_x + 7, torso_y + 22], PAL_GOLD[1])
    draw.line([(sh_x - 12, torso_y + 15), (sh_x + 12, torso_y + 15)], fill=PAL_GOLD[0], width=3)
    draw.line([(sh_x, torso_y - 10), (sh_x, torso_y + 40)], fill=PAL_GOLD[0], width=3)

    # 5. Gladius Sword Strike
    if is_attack:
        draw_capcom_muscle_limb(draw, (cx - 18, torso_y), (cx - 46, torso_y - 10), 8, 6, PAL_SKIN)
        # Steel blade thrust
        draw.line([(cx - 46, torso_y - 10), (cx - 76, torso_y - 16)], fill=PAL_STEEL[0], width=6)
        draw.line([(cx - 46, torso_y - 9), (cx - 76, torso_y - 15)], fill=PAL_STEEL[1], width=2)
        # Pommel & guard
        draw.ellipse([cx - 48, torso_y - 14, cx - 42, torso_y - 6], fill=PAL_BRONZE[0])
    else:
        draw_capcom_muscle_limb(draw, (cx - 18, torso_y), (cx - 26, torso_y + 24), 7, 6, PAL_SKIN)

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_roman_archer(frame_name, is_fire=False, is_duck=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y = 82 if not is_duck else 102
    head_y = 44 if not is_duck else 64
    if is_hurt: cx -= 6

    # Archer Leather Galea
    draw_ink_circle(draw, [cx - 14, head_y - 14, cx + 14, head_y + 10], (125, 80, 45, 255))
    draw.arc([cx - 12, head_y - 12, cx + 12, head_y + 6], 0, 180, fill=PAL_BRONZE[1], width=3)
    draw.rectangle([cx - 8, head_y, cx + 8, head_y + 8], fill=PAL_SKIN[2])

    # Leather Scale Brigandine
    draw_ink_poly(draw, [(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 16, torso_y + 34), (cx - 16, torso_y + 34)], (145, 95, 55, 255))
    for y in range(torso_y - 4, torso_y + 30, 6):
        draw.line([(cx - 14, y), (cx + 14, y)], fill=(185, 130, 75, 255), width=2)

    # Legs
    if is_duck:
        draw_ink_circle(draw, [cx - 24, torso_y + 16, cx + 24, ground_y], (90, 55, 30, 255))
    else:
        draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 34), (cx - 10, ground_y - 6), 6, 5, PAL_SKIN)
        draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 34), (cx + 10, ground_y - 6), 6, 5, PAL_SKIN)

    # Recurve Composite Bow
    bow_x = cx + 26
    draw.arc([bow_x - 22, torso_y - 24, bow_x + 22, torso_y + 44], 270, 90, fill=(160, 105, 40, 255), width=5)
    draw.line([(bow_x, torso_y - 24), (bow_x, torso_y + 44)], fill=(230, 230, 240, 240), width=2)
    if is_fire:
        draw.line([(bow_x - 30, torso_y + 10), (bow_x + 18, torso_y + 10)], fill=(220, 220, 230, 255), width=3)
        draw.polygon([(bow_x + 18, torso_y + 7), (bow_x + 26, torso_y + 10), (bow_x + 18, torso_y + 13)], fill=PAL_STEEL[0])

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_roman_heavy(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 84, 44
    if is_hurt: cx -= 6

    # Heavy Bronze Galea with Black Crest
    draw_ink_circle(draw, [cx - 24, head_y - 32, cx + 24, head_y - 14], (30, 25, 35, 255))
    draw_ink_circle(draw, [cx - 18, head_y - 16, cx + 18, head_y + 12], PAL_BRONZE[1])

    # Musculata Cuirass (Chiseled Pectorals & Abs in Solid Bronze)
    draw_ink_poly(draw, [(cx - 26, torso_y - 12), (cx + 26, torso_y - 12), (cx + 22, torso_y + 38), (cx - 22, torso_y + 38)], PAL_BRONZE[2])
    draw.arc([cx - 20, torso_y - 4, cx - 2, torso_y + 14], 0, 180, fill=PAL_BRONZE[0], width=3)
    draw.arc([cx + 2, torso_y - 4, cx + 20, torso_y + 14], 0, 180, fill=PAL_BRONZE[0], width=3)

    # Greaves & Stride
    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 12, torso_y + 38), (cx - 12 + int(stride), ground_y - 6), 9, 7, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 12, torso_y + 38), (cx + 12 - int(stride), ground_y - 6), 9, 7, PAL_BRONZE)

    # Pilum (Heavy Spear)
    spear_x = cx - 34 if not is_attack else cx - 58
    draw.line([(spear_x, 14), (spear_x, ground_y)], fill=(120, 80, 40, 255), width=6)
    draw.line([(spear_x, 14), (spear_x, -16)], fill=PAL_STEEL[1], width=4)
    draw.polygon([(spear_x - 4, -16), (spear_x + 4, -16), (spear_x, -28)], fill=PAL_STEEL[0])

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_centurion_brutus(frame_name, walk_phase=0, is_whip_prepare=False, is_whip_crack=False, is_whip_slam=False, is_hurt=False):
    # Boss: Centurion Brutus (Huge 176x208 Capcom Boss scale)
    W, H = 176, 208
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 92, 48
    if is_hurt: cx -= 8

    # Transverse Imperial Crest (Side-to-Side Crest)
    draw_ink_circle(draw, [cx - 36, head_y - 36, cx + 36, head_y - 16], PAL_CRIMSON[1])
    draw.ellipse([cx - 32, head_y - 34, cx + 32, head_y - 18], fill=PAL_CRIMSON[0])

    # Gilded Centurion Helmet with Laurel Wreath
    draw_ink_circle(draw, [cx - 20, head_y - 18, cx + 20, head_y + 14], PAL_GOLD[1])
    draw.arc([cx - 18, head_y - 16, cx + 18, head_y + 8], 0, 180, fill=PAL_GOLD[0], width=3)

    # Gilded Muscle Cuirass with Gorgon Medallion
    draw_ink_poly(draw, [(cx - 30, torso_y - 14), (cx + 30, torso_y - 14), (cx + 26, torso_y + 42), (cx - 26, torso_y + 42)], PAL_GOLD[2])
    draw.arc([cx - 24, torso_y - 4, cx - 2, torso_y + 16], 0, 180, fill=PAL_GOLD[0], width=3)
    draw.arc([cx + 2, torso_y - 4, cx + 24, torso_y + 16], 0, 180, fill=PAL_GOLD[0], width=3)

    # Imperial Scarlet Paludamentum Cape
    draw_ink_poly(draw, [(cx - 28, torso_y - 10), (cx - 62, torso_y + 68), (cx - 22, ground_y - 14), (cx + 28, torso_y - 10)], PAL_CRIMSON[2])

    # Greaved Legs & Stride
    stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 14, torso_y + 42), (cx - 16 + int(stride), ground_y - 6), 11, 8, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 14, torso_y + 42), (cx + 16 - int(stride), ground_y - 6), 11, 8, PAL_BRONZE)

    # Flagrum Scorpion Whip
    if is_whip_crack or is_whip_slam:
        whip_tip_x = cx - 68
        whip_tip_y = torso_y + 18
        draw_capcom_muscle_limb(draw, (cx - 24, torso_y), (cx - 46, torso_y - 16), 10, 8, PAL_SKIN)
        draw_capcom_muscle_limb(draw, (cx - 46, torso_y - 16), (cx - 62, torso_y), 9, 7, PAL_SKIN)
        for t in range(-8, 9, 8):
            draw.line([(cx - 62, torso_y), (whip_tip_x, whip_tip_y + t * 4)], fill=PAL_STEEL[1], width=4)
            draw_ink_circle(draw, [whip_tip_x - 5, whip_tip_y + t * 4 - 5, whip_tip_x + 5, whip_tip_y + t * 4 + 5], PAL_STEEL[0])
    else:
        draw_capcom_muscle_limb(draw, (cx - 24, torso_y), (cx - 34, torso_y + 28), 10, 8, PAL_SKIN)
        draw.line([(cx - 34, torso_y + 28), (cx - 38, ground_y - 12)], fill=PAL_STEEL[2], width=4)

    if is_hurt:
        draw.line([(cx - 16, head_y - 4), (cx + 16, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_sicarii(frame_name, is_slash=False, is_throw=False, is_cling=False):
    W, H = 144, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 74, 40

    # Hooded Midnight Assassin Cowl
    draw_ink_poly(draw, [(cx - 20, head_y - 18), (cx + 20, head_y - 18), (cx + 16, head_y + 20), (cx - 16, head_y + 20)], (32, 28, 48, 255))
    # Glowing predatory yellow eyes
    draw.ellipse([cx - 9, head_y + 2, cx - 4, head_y + 7], fill=(255, 230, 80, 255))
    draw.ellipse([cx + 4, head_y + 2, cx + 9, head_y + 7], fill=(255, 230, 80, 255))

    # Agile Dark Ninja Tunic
    draw_ink_poly(draw, [(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 20, torso_y + 34), (cx - 20, torso_y + 34)], (46, 40, 64, 255))

    # Legs & Ninja Boots
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 34), (cx - 10, ground_y - 6), 7, 5, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 34), (cx + 10, ground_y - 6), 7, 5, PAL_SKIN)

    # Curved Sica Steel Blades
    draw.arc([cx - 36, torso_y - 12, cx - 14, torso_y + 24], 90, 270, fill=PAL_STEEL[0], width=4)
    draw.arc([cx + 14, torso_y - 12, cx + 36, torso_y + 24], 270, 90, fill=PAL_STEEL[0], width=4)

    save_sprite(img, frame_name)

def create_capcom_temple_boss(frame_name, walk_phase=0, is_slash=False, is_slam=False, is_hurt=False):
    W, H = 176, 208
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 92, 48
    if is_hurt: cx -= 8

    # Menorah Golden High-Priest Guard Helmet
    draw_ink_circle(draw, [cx - 24, head_y - 28, cx + 24, head_y + 14], PAL_GOLD[1])
    draw.line([(cx, head_y - 24), (cx, head_y - 4)], fill=PAL_GOLD[0], width=4)
    draw_ink_poly(draw, [(cx - 14, head_y + 6), (cx + 14, head_y + 6), (cx, head_y + 26)], PAL_AUBURN[2])

    # Temple Cuirass & Purple Sash
    draw_ink_poly(draw, [(cx - 30, torso_y - 14), (cx + 30, torso_y - 14), (cx + 26, torso_y + 44), (cx - 26, torso_y + 44)], PAL_GOLD[2])
    draw_ink_poly(draw, [(cx - 28, torso_y + 28), (cx + 28, torso_y + 32), (cx + 24, torso_y + 44), (cx - 24, torso_y + 40)], (120, 25, 110, 255))

    # Greaved Legs
    stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 16, torso_y + 44), (cx - 16 + int(stride), ground_y - 6), 11, 8, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 16, torso_y + 44), (cx + 16 - int(stride), ground_y - 6), 11, 8, PAL_BRONZE)

    # Spiked Morningstar Golden Mace
    mace_x = cx - 56 if (is_slash or is_slam) else cx - 40
    mace_y = torso_y - 14 if (is_slash or is_slam) else ground_y - 16
    draw.line([(cx - 24, torso_y), (mace_x, mace_y)], fill=(140, 95, 45, 255), width=7)
    draw_ink_circle(draw, [mace_x - 16, mace_y - 16, mace_x + 16, mace_y + 16], PAL_GOLD[0])
    for angle in range(0, 360, 45):
        rad = math.radians(angle)
        draw.line([(mace_x, mace_y), (mace_x + math.cos(rad) * 24, mace_y + math.sin(rad) * 24)], fill=PAL_GOLD[0], width=3)

    if is_hurt:
        draw.line([(cx - 16, head_y - 4), (cx + 16, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_tribunal_boss(frame_name, walk_phase=0, is_slash=False, is_slam=False, is_hurt=False):
    W, H = 176, 208
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 92, 48
    if is_hurt: cx -= 8

    # Corinthian Praetor Helmet & Crest
    draw_ink_poly(draw, [(cx - 28, head_y - 36), (cx + 28, head_y - 36), (cx, head_y - 18)], PAL_CRIMSON[1])
    draw_ink_circle(draw, [cx - 20, head_y - 20, cx + 20, head_y + 14], PAL_GOLD[1])

    # Muscle Armor with Lion Pauldrons
    draw_ink_poly(draw, [(cx - 30, torso_y - 14), (cx + 30, torso_y - 14), (cx + 26, torso_y + 44), (cx - 26, torso_y + 44)], PAL_GOLD[2])
    draw_ink_circle(draw, [cx - 38, torso_y - 18, cx - 20, torso_y + 6], PAL_GOLD[0])
    draw_ink_circle(draw, [cx + 20, torso_y - 18, cx + 38, torso_y + 6], PAL_GOLD[0])

    # Scarlet Cape
    draw_ink_poly(draw, [(cx - 28, torso_y - 10), (cx - 64, torso_y + 70), (cx - 24, ground_y - 14), (cx + 28, torso_y - 10)], PAL_CRIMSON[2])

    stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 16, torso_y + 44), (cx - 16 + int(stride), ground_y - 6), 11, 8, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 16, torso_y + 44), (cx + 16 - int(stride), ground_y - 6), 11, 8, PAL_BRONZE)

    # Holy Lance (Pilum of Longinus)
    draw.line([(cx + 28, torso_y - 60), (cx + 28, ground_y)], fill=(120, 75, 35, 255), width=6)
    draw_ink_poly(draw, [(cx + 22, torso_y - 60), (cx + 34, torso_y - 60), (cx + 28, torso_y - 88)], PAL_STEEL[0])

    if is_hurt:
        draw.line([(cx - 16, head_y - 4), (cx + 16, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

# Generate Mission 1
create_capcom_roman_guard("spr_guard_walk_0", walk_phase=0)
create_capcom_roman_guard("spr_guard_walk_1", walk_phase=1)
create_capcom_roman_guard("spr_guard_attack", is_attack=True)
create_capcom_roman_guard("spr_guard_hurt", is_hurt=True)

create_capcom_roman_archer("spr_archer_stand")
create_capcom_roman_archer("spr_archer_duck", is_duck=True)
create_capcom_roman_archer("spr_archer_fire", is_fire=True)
create_capcom_roman_archer("spr_archer_hurt", is_hurt=True)

create_capcom_roman_heavy("spr_heavy_walk_0", walk_phase=0)
create_capcom_roman_heavy("spr_heavy_walk_1", walk_phase=1)
create_capcom_roman_heavy("spr_heavy_attack", is_attack=True)
create_capcom_roman_heavy("spr_heavy_hurt", is_hurt=True)

create_capcom_centurion_brutus("spr_boss_idle")
create_capcom_centurion_brutus("spr_boss_walk_0", walk_phase=0)
create_capcom_centurion_brutus("spr_boss_walk_1", walk_phase=1)
create_capcom_centurion_brutus("spr_boss_whip_prepare", is_whip_prepare=True)
create_capcom_centurion_brutus("spr_boss_whip_crack", is_whip_crack=True)
create_capcom_centurion_brutus("spr_boss_whip_slam", is_whip_slam=True)
create_capcom_centurion_brutus("spr_boss_hurt", is_hurt=True)

create_capcom_sicarii("spr_sicarii_stand")
create_capcom_sicarii("spr_sicarii_slash", is_slash=True)
create_capcom_sicarii("spr_sicarii_throw", is_throw=True)
create_capcom_sicarii("spr_sicarii_wall_cling", is_cling=True)
create_capcom_sicarii("spr_sicarii_hurt")

create_capcom_temple_boss("spr_temple_boss_idle")
create_capcom_temple_boss("spr_temple_boss_walk_0", walk_phase=0)
create_capcom_temple_boss("spr_temple_boss_walk_1", walk_phase=1)
create_capcom_temple_boss("spr_temple_boss_slash", is_slash=True)
create_capcom_temple_boss("spr_temple_boss_slam", is_slam=True)
create_capcom_temple_boss("spr_temple_boss_hurt", is_hurt=True)

create_capcom_tribunal_boss("spr_tribunal_idle")
create_capcom_tribunal_boss("spr_tribunal_walk_0", walk_phase=0)
create_capcom_tribunal_boss("spr_tribunal_walk_1", walk_phase=1)
create_capcom_tribunal_boss("spr_tribunal_slash", is_slash=True)
create_capcom_tribunal_boss("spr_tribunal_slam", is_slam=True)
create_capcom_tribunal_boss("spr_tribunal_hurt", is_hurt=True)


# =========================================================================
# PART 4: BONUS STAGE (BEATITUDES VILLAGERS & JESUS BASKET)
# =========================================================================
def create_capcom_villager(frame_name, vtype="elder", run_phase=0, is_satisfied=False):
    W, H = 128, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 74, 40

    stride = math.sin(run_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 34), (cx - 10 + int(stride), ground_y - 6), 6, 5, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 34), (cx + 10 - int(stride), ground_y - 6), 6, 5, PAL_SKIN)

    if vtype == "elder":
        draw_ink_circle(draw, [cx - 14, head_y - 14, cx + 14, head_y + 10], PAL_SKIN[2])
        draw_ink_poly(draw, [(cx - 12, head_y + 4), (cx + 12, head_y + 4), (cx, head_y + 28)], (240, 240, 245, 255)) # White beard
        draw_ink_poly(draw, [(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 20, torso_y + 34), (cx - 20, torso_y + 34)], (130, 95, 60, 255))
    elif vtype == "woman":
        draw_ink_circle(draw, [cx - 16, head_y - 16, cx + 16, head_y + 16], (240, 240, 245, 255)) # White scarf
        draw_ink_circle(draw, [cx - 12, head_y - 10, cx + 12, head_y + 10], PAL_SKIN[1])
        draw_ink_poly(draw, [(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 22, torso_y + 34), (cx - 22, torso_y + 34)], (45, 125, 135, 255))
    elif vtype == "youth":
        draw_ink_circle(draw, [cx - 12, head_y - 12, cx + 12, head_y + 10], PAL_SKIN[1])
        draw_ink_poly(draw, [(cx - 16, torso_y - 8), (cx + 16, torso_y - 8), (cx + 18, torso_y + 32), (cx - 18, torso_y + 32)], (65, 135, 65, 255))
    else: # beggar
        draw_ink_circle(draw, [cx - 12, head_y - 12, cx + 12, head_y + 10], PAL_SKIN[3])
        draw_ink_poly(draw, [(cx - 16, torso_y - 8), (cx + 16, torso_y - 8), (cx + 18, torso_y + 32), (cx - 18, torso_y + 32)], (110, 100, 85, 255))

    if is_satisfied:
        # Joyful golden bread & fish in hands
        draw_ink_circle(draw, [cx - 10, torso_y + 4, cx + 10, torso_y + 20], (240, 180, 80, 255))
        # Radiant blessing aura
        for spx, spy in [(-20, head_y - 16), (20, head_y - 16), (0, head_y - 24)]:
            draw.ellipse([cx + spx - 4, spy - 4, cx + spx + 4, spy + 4], fill=(255, 255, 180, 255))

    save_sprite(img, frame_name)

def create_capcom_jesus_basket(frame_name, is_throw=False):
    W, H = 160, 200
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 98, 52

    # Nimbus Halo
    for r, col in [(42, (255, 215, 60, 50)), (34, (255, 230, 100, 100)), (26, (255, 250, 180, 160))]:
        draw.ellipse([cx - r, head_y - r - 4, cx + r, head_y + r - 4], fill=col)
    draw.ellipse([cx - 28, head_y - 32, cx + 28, head_y + 24], outline=C_INK, width=4)

    # Head & Robes
    draw_ink_poly(draw, [(cx - 14, head_y - 14), (cx + 14, head_y - 14), (cx + 12, head_y + 16), (cx, head_y + 24), (cx - 12, head_y + 16)], PAL_SKIN[1])
    draw_ink_poly(draw, [(cx - 13, head_y + 4), (cx + 13, head_y + 4), (cx + 10, head_y + 22), (cx, head_y + 28), (cx - 10, head_y + 22)], PAL_AUBURN[2])
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y - 14), (cx + 22, torso_y + 46), (cx - 22, torso_y + 46)], PAL_WRAP[1])
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y + 12), (cx + 22, torso_y + 46), (cx - 16, torso_y + 46)], PAL_CRIMSON[2])

    # Legs
    draw_ink_poly(draw, [(cx - 24, torso_y + 44), (cx - 36, ground_y - 20), (cx - 18, ground_y - 20), (cx - 10, torso_y + 44)], PAL_WRAP[2])
    draw_ink_poly(draw, [(cx + 10, torso_y + 44), (cx + 18, ground_y - 20), (cx + 38, ground_y - 20), (cx + 24, torso_y + 44)], PAL_WRAP[3])

    # Wicker Basket in Hands
    bx = cx + 28 if not is_throw else cx - 36
    by = torso_y + 16 if not is_throw else torso_y - 10
    draw_ink_poly(draw, [(bx - 16, by), (bx + 16, by), (bx + 12, by + 22), (bx - 12, by + 22)], (175, 120, 60, 255))
    # Loaves & Fishes
    draw.ellipse([bx - 8, by - 6, bx, by + 6], fill=(215, 155, 80, 255))
    draw_ink_poly(draw, [(bx, by), (bx + 12, by - 6), (bx + 6, by + 6)], (120, 185, 220, 255))

    save_sprite(img, frame_name)

for vtype in ["elder", "woman", "youth", "beggar"]:
    create_capcom_villager(f"spr_villager_{vtype}_run_0", vtype=vtype, run_phase=0)
    create_capcom_villager(f"spr_villager_{vtype}_run_1", vtype=vtype, run_phase=1)
create_capcom_villager("spr_villager_satisfied", is_satisfied=True)
create_capcom_jesus_basket("spr_jesus_basket_aim", is_throw=False)
create_capcom_jesus_basket("spr_jesus_basket_throw", is_throw=True)

# =========================================================================
# PART 5: MISSION 2 (GETHSEMANE ENEMIES & BOSSES)
# =========================================================================
def create_capcom_tormentor(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 82, 42
    if is_hurt: cx -= 6

    # Scarred Hooded Executioner
    draw_ink_poly(draw, [(cx - 16, head_y - 18), (cx + 16, head_y - 18), (cx + 14, head_y + 16), (cx - 14, head_y + 16)], (65, 45, 35, 255))
    draw_ink_circle(draw, [cx - 12, head_y - 8, cx + 12, head_y + 10], PAL_SKIN[3])
    draw.line([(cx - 8, head_y - 4), (cx + 6, head_y + 8)], fill=(190, 40, 40, 255), width=3) # Wicked scar

    # Studded Leather Vest
    draw_ink_poly(draw, [(cx - 20, torso_y - 8), (cx + 20, torso_y - 8), (cx + 22, torso_y + 36), (cx - 22, torso_y + 36)], (75, 45, 30, 255))
    for y in range(torso_y, torso_y + 32, 8):
        draw.point((cx - 10, y), fill=PAL_STEEL[0])
        draw.point((cx + 10, y), fill=PAL_STEEL[0])

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 36), (cx - 10 + int(stride), ground_y - 6), 7, 6, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 36), (cx + 10 - int(stride), ground_y - 6), 7, 6, PAL_SKIN)

    # Flaming Pitch Torch (Animated Dynamic Fire Corona)
    torch_x, torch_y = cx + 32, torso_y - 14
    draw.line([(cx + 18, torso_y), (torch_x, torch_y + 24)], fill=(110, 70, 35, 255), width=5)
    for r, col in [(20, (255, 90, 20, 90)), (14, (255, 170, 30, 160)), (8, (255, 245, 120, 230))]:
        draw.ellipse([torch_x - r, torch_y - r, torch_x + r, torch_y + r], fill=col)

    # Spiked Heavy Cudgel
    cudgel_x = cx - 32 if not is_attack else cx - 54
    draw.line([(cx - 18, torso_y), (cudgel_x, torso_y - 16)], fill=(80, 50, 25, 255), width=7)
    draw_ink_circle(draw, [cudgel_x - 10, torso_y - 26, cudgel_x + 10, torso_y - 6], PAL_STEEL[2])

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_thorn_sicarii(frame_name, is_crouch=False, is_leap=False, is_slash=False, is_hurt=False):
    W, H = 144, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = (74, 40) if not is_crouch else (94, 60)
    if is_leap: torso_y -= 20; head_y -= 20
    if is_hurt: cx -= 6

    # Bramble Camo Ivy Cowl
    draw_ink_poly(draw, [(cx - 20, head_y - 18), (cx + 20, head_y - 18), (cx + 16, head_y + 20), (cx - 16, head_y + 20)], (38, 58, 32, 255))
    # Glowing predatory emerald eyes
    draw.ellipse([cx - 9, head_y + 2, cx - 4, head_y + 7], fill=(120, 255, 140, 255))
    draw.ellipse([cx + 4, head_y + 2, cx + 9, head_y + 7], fill=(120, 255, 140, 255))

    # Tunic with Thorny Bramble Wraps
    draw_ink_poly(draw, [(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 20, torso_y + 34), (cx - 20, torso_y + 34)], (48, 68, 38, 255))

    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 34), (cx - 10, ground_y - 6), 7, 5, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 34), (cx + 10, ground_y - 6), 7, 5, PAL_SKIN)

    # Serrated Poison Thorn Blades
    draw_ink_poly(draw, [(cx - 36, torso_y - 16), (cx - 16, torso_y), (cx - 44, torso_y + 10)], (130, 215, 150, 255))
    draw_ink_poly(draw, [(cx + 36, torso_y - 16), (cx + 16, torso_y), (cx + 44, torso_y + 10)], (130, 215, 150, 255))

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_corrupted_priest(frame_name, is_cast=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 80, 42
    if is_hurt: cx -= 6

    # Dark Ritualistic Talit / Cowl
    draw_ink_poly(draw, [(cx - 22, head_y - 20), (cx + 22, head_y - 20), (cx + 18, head_y + 26), (cx - 18, head_y + 26)], (42, 28, 55, 255))
    # Pallid face & glowing demonic purple eyes
    draw_ink_circle(draw, [cx - 12, head_y - 10, cx + 12, head_y + 12], (190, 180, 195, 255))
    draw.ellipse([cx - 8, head_y - 2, cx - 3, head_y + 3], fill=(220, 40, 255, 255))
    draw.ellipse([cx + 3, head_y - 2, cx + 8, head_y + 3], fill=(220, 40, 255, 255))

    # Tattered Charcoal Robes
    draw_ink_poly(draw, [(cx - 20, torso_y - 8), (cx + 20, torso_y - 8), (cx + 26, ground_y - 4), (cx - 26, ground_y - 4)], (32, 22, 44, 255))

    # Dark Occult Swirling Energy Orb
    orb_x = cx - 36 if is_cast else cx - 22
    orb_y = torso_y - 10
    draw_capcom_muscle_limb(draw, (cx - 16, torso_y), (orb_x + 10, orb_y + 14), 6, 5, (180, 175, 190, 255))
    for r, col in [(22, (160, 30, 220, 80)), (14, (200, 60, 255, 170)), (7, (255, 220, 255, 240))]:
        draw.ellipse([orb_x - r, orb_y - r, orb_x + r, orb_y + r], fill=col)

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_clone_boss(frame_name, walk_phase=0, is_channel=False, is_hurt=False):
    W, H = 160, 200
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 98, 52
    if is_hurt: cx -= 8

    # Inverted Violet Demonic Halo
    halo_r = 30
    for r in range(halo_r, 0, -3):
        draw.ellipse([cx - r, head_y - r - 4, cx + r, head_y + r - 4], fill=(140, 20, 220, 50))
    draw.ellipse([cx - halo_r, head_y - halo_r - 4, cx + halo_r, head_y + halo_r - 4], outline=(240, 80, 255, 220), width=3)

    # Pallid Head & Blood-Red Eyes
    draw_ink_poly(draw, [(cx - 14, head_y - 14), (cx + 14, head_y - 14), (cx + 12, head_y + 16), (cx, head_y + 24), (cx - 12, head_y + 16)], (200, 195, 210, 255))
    draw_ink_poly(draw, [(cx - 13, head_y + 4), (cx + 13, head_y + 4), (cx + 10, head_y + 22), (cx, head_y + 28), (cx - 10, head_y + 22)], (30, 24, 34, 255))
    draw.ellipse([cx - 8, head_y - 2, cx - 2, head_y + 3], fill=(255, 30, 40, 255))
    draw.ellipse([cx + 2, head_y - 2, cx + 8, head_y + 3], fill=(255, 30, 40, 255))

    # Obsidian / Charcoal Tunic & Crimson Stole
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y - 14), (cx + 22, torso_y + 46), (cx - 22, torso_y + 46)], (32, 28, 38, 255))
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y + 12), (cx + 22, torso_y + 46), (cx - 16, torso_y + 46)], (125, 12, 28, 255))

    stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 44), (cx - 10 + int(stride), ground_y - 6), 8, 6, (190, 185, 200, 255))
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 44), (cx + 10 - int(stride), ground_y - 6), 8, 6, (190, 185, 200, 255))

    # Corrupted Scourge Whip
    draw_capcom_muscle_limb(draw, (cx + 18, torso_y), (cx + 42, torso_y + 10), 8, 6, (190, 185, 200, 255))
    draw.line([(cx + 42, torso_y + 10), (cx + 68, torso_y - 12)], fill=(160, 30, 220, 255), width=4)
    draw_ink_circle(draw, [cx + 64, torso_y - 16, cx + 74, torso_y - 6], (255, 40, 60, 255))

    if is_hurt:
        draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

create_capcom_tormentor("spr_tormentor_walk_0", walk_phase=0)
create_capcom_tormentor("spr_tormentor_walk_1", walk_phase=1)
create_capcom_tormentor("spr_tormentor_attack", is_attack=True)
create_capcom_tormentor("spr_tormentor_hurt", is_hurt=True)

create_capcom_thorn_sicarii("spr_thorn_sicarii_crouch", is_crouch=True)
create_capcom_thorn_sicarii("spr_thorn_sicarii_leap", is_leap=True)
create_capcom_thorn_sicarii("spr_thorn_sicarii_slash", is_slash=True)
create_capcom_thorn_sicarii("spr_thorn_sicarii_hurt", is_hurt=True)

create_capcom_corrupted_priest("spr_corrupted_priest_stand")
create_capcom_corrupted_priest("spr_corrupted_priest_cast", is_cast=True)
create_capcom_corrupted_priest("spr_corrupted_priest_hurt", is_hurt=True)

create_capcom_clone_boss("spr_clone_idle")
create_capcom_clone_boss("spr_clone_walk_0", walk_phase=0)
create_capcom_clone_boss("spr_clone_walk_1", walk_phase=1)
create_capcom_clone_boss("spr_clone_channel", is_channel=True)
create_capcom_clone_boss("spr_clone_hurt", is_hurt=True)


# =========================================================================
# PART 6: MISSION 3 (THE OLD CITY & PALACE ENEMIES & BOSSES)
# =========================================================================
def create_capcom_money_changer(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 82, 44
    if is_hurt: cx -= 6

    # Turban with Gold Jewel
    draw_ink_circle(draw, [cx - 20, head_y - 24, cx + 20, head_y + 2], (245, 240, 230, 255))
    draw_ink_circle(draw, [cx - 6, head_y - 16, cx + 6, head_y - 4], PAL_GOLD[0])

    # Sly Merchant Face with Pointed Beard
    draw_ink_circle(draw, [cx - 14, head_y - 10, cx + 14, head_y + 12], PAL_SKIN[2])
    draw_ink_poly(draw, [(cx - 10, head_y + 8), (cx + 10, head_y + 8), (cx, head_y + 26)], PAL_AUBURN[2])
    draw.point((cx - 5, head_y), fill=(20, 20, 20, 255))
    draw.point((cx + 5, head_y), fill=(20, 20, 20, 255))

    # Rotund Striped Silk Robe (Saffron & Tyrian Purple)
    draw_ink_poly(draw, [(cx - 24, torso_y - 8), (cx + 24, torso_y - 8), (cx + 28, torso_y + 42), (cx - 28, torso_y + 42)], (215, 175, 45, 255))
    for sx in range(cx - 18, cx + 22, 9):
        draw.line([(sx, torso_y - 6), (sx, torso_y + 40)], fill=(120, 20, 95, 255), width=4)

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 42), (cx - 10 + int(stride), ground_y - 6), 7, 6, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 42), (cx + 10 - int(stride), ground_y - 6), 7, 6, PAL_SKIN)

    # Heavy Leather Coin Bag
    bag_x = cx + 32 if not is_attack else cx - 44
    bag_y = torso_y + 18 if not is_attack else torso_y - 12
    draw_capcom_muscle_limb(draw, (cx + 16, torso_y), (bag_x, bag_y), 7, 5, PAL_SKIN)
    draw_ink_circle(draw, [bag_x - 14, bag_y - 6, bag_x + 14, bag_y + 24], (130, 80, 45, 255))
    draw.line([(bag_x - 8, bag_y - 4), (bag_x + 8, bag_y - 4)], fill=PAL_GOLD[0], width=3)

    if is_attack:
        for dx, dy in [(-16, -10), (-28, 6), (-12, 20), (-22, -22)]:
            draw_ink_circle(draw, [bag_x + dx - 5, bag_y + dy - 5, bag_x + dx + 5, bag_y + dy + 5], PAL_GOLD[0])

    if is_hurt:
        draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 40, 40, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_levite_enforcer(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 86, 44
    if is_hurt: cx -= 6

    # Conical Temple Bronze Helmet
    draw_ink_poly(draw, [(cx - 20, head_y - 28), (cx + 20, head_y - 28), (cx, head_y - 50)], PAL_BRONZE[1])
    draw_ink_circle(draw, [cx - 16, head_y - 16, cx + 16, head_y + 12], PAL_BRONZE[2])

    draw_ink_circle(draw, [cx - 12, head_y - 6, cx + 12, head_y + 12], PAL_SKIN[2])
    draw_ink_poly(draw, [(cx - 12, head_y + 6), (cx + 12, head_y + 6), (cx, head_y + 26)], PAL_AUBURN[2])

    # Deep Celestial Blue & White Pleated Tunic
    draw_ink_poly(draw, [(cx - 24, torso_y - 10), (cx + 24, torso_y - 10), (cx + 26, torso_y + 40), (cx - 26, torso_y + 40)], PAL_COBALT[2])
    draw.line([(cx - 22, torso_y - 6), (cx + 22, torso_y - 6)], fill=PAL_GOLD[0], width=4)
    draw.line([(cx, torso_y - 6), (cx, torso_y + 38)], fill=PAL_GOLD[1], width=3)

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 12, torso_y + 40), (cx - 12 + int(stride), ground_y - 6), 8, 6, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 12, torso_y + 40), (cx + 12 - int(stride), ground_y - 6), 8, 6, PAL_BRONZE)

    # Spiked Bronze Staff / Truncheon
    staff_x = cx + 34 if not is_attack else cx - 48
    draw.line([(staff_x, 14), (staff_x, ground_y)], fill=(120, 80, 45, 255), width=6)
    draw_ink_circle(draw, [staff_x - 14, 10, staff_x + 14, 40], PAL_BRONZE[0])
    for dy in range(14, 38, 7):
        draw.line([(staff_x - 18, dy), (staff_x + 18, dy)], fill=PAL_BRONZE[1], width=3)

    if is_hurt:
        draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_barabbas_boss(frame_name, walk_phase=0, is_slash=False, is_slam=False, is_hurt=False):
    # Massive Capcom Brawler Boss (180x216 scale - direct tribute to the user's reference image!)
    W, H = 180, 216
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 96, 48
    if is_hurt: cx -= 8

    # 1. Wild Unkempt Mane & Fierce Beard
    draw_ink_circle(draw, [cx - 30, head_y - 24, cx + 30, head_y + 32], (45, 28, 16, 255))
    draw_ink_circle(draw, [cx - 16, head_y - 12, cx + 16, head_y + 14], PAL_SKIN_BRONZE[3])
    # Menacing Scowl, Brow Ridge & Grimacing White Teeth
    draw.line([(cx - 10, head_y - 4), (cx - 2, head_y - 2)], fill=C_INK, width=3)
    draw.line([(cx + 2, head_y - 2), (cx + 10, head_y - 4)], fill=C_INK, width=3)
    draw.rectangle([cx - 6, head_y + 10, cx + 6, head_y + 15], fill=C_INK)
    draw.rectangle([cx - 5, head_y + 11, cx + 5, head_y + 13], fill=(255, 255, 255, 255)) # Teeth!
    draw_ink_poly(draw, [(cx - 18, head_y + 8), (cx + 18, head_y + 8), (cx, head_y + 34)], (45, 28, 16, 255))

    # 2. Muscular Bare Torso (Chiseled Pecs, 6-Pack Abs, Linea Alba, Battle Scars!)
    draw_ink_poly(draw, [(cx - 32, torso_y - 14), (cx + 32, torso_y - 14), (cx + 26, torso_y + 44), (cx - 26, torso_y + 44)], PAL_SKIN_BRONZE[2])
    # Pectorals separation & drop shadow
    draw.arc([cx - 26, torso_y - 4, cx - 2, torso_y + 18], 0, 180, fill=PAL_SKIN_BRONZE[0], width=3)
    draw.arc([cx + 2, torso_y - 4, cx + 26, torso_y + 18], 0, 180, fill=PAL_SKIN_BRONZE[0], width=3)
    # Linea alba (Center abdominal crease)
    draw.line([(cx, torso_y + 8), (cx, torso_y + 42)], fill=PAL_SKIN_BRONZE[4], width=2)
    # 6-Pack abdominal horizontal cuts
    for ay in [torso_y + 20, torso_y + 30, torso_y + 40]:
        draw.line([(cx - 18, ay), (cx - 3, ay)], fill=PAL_SKIN_BRONZE[3], width=2)
        draw.line([(cx + 3, ay), (cx + 18, ay)], fill=PAL_SKIN_BRONZE[3], width=2)
    # Chest hair & cross scar
    draw.line([(cx - 16, torso_y - 6), (cx + 12, torso_y + 14)], fill=(180, 40, 40, 255), width=3) # Wicked battle scar

    # Heavy Leather Harness
    draw.line([(cx - 28, torso_y - 10), (cx + 24, torso_y + 42)], fill=(75, 45, 25, 255), width=5)

    # 3. Tactical Desert Trousers & Combat Boots (Matching user image!)
    draw_ink_poly(draw, [(cx - 28, torso_y + 42), (cx + 28, torso_y + 42), (cx + 24, ground_y - 16), (cx - 24, ground_y - 16)], (195, 180, 135, 255))
    # Cargo pockets
    draw_ink_poly(draw, [(cx - 32, torso_y + 54), (cx - 22, torso_y + 54), (cx - 22, torso_y + 70), (cx - 32, torso_y + 70)], (165, 150, 110, 255))
    draw_ink_poly(draw, [(cx + 22, torso_y + 54), (cx + 32, torso_y + 54), (cx + 32, torso_y + 70), (cx + 22, torso_y + 70)], (165, 150, 110, 255))

    stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 14, ground_y - 20), (cx - 16 + int(stride), ground_y - 4), 10, 8, PAL_SKIN_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 14, ground_y - 20), (cx + 16 - int(stride), ground_y - 4), 10, 8, PAL_SKIN_BRONZE)

    # 4. Massive Muscular Arms, Broken Iron Manacles & Dangling Chains!
    arm_l = (cx - 48, torso_y + 10) if not is_slash else (cx - 68, torso_y - 20)
    arm_r = (cx + 48, torso_y + 10) if not is_slash else (cx + 68, torso_y - 20)
    draw_capcom_muscle_limb(draw, (cx - 28, torso_y), arm_l, 12, 9, PAL_SKIN_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 28, torso_y), arm_r, 12, 9, PAL_SKIN_BRONZE)
    # Heavy iron wrist cuffs
    draw.rectangle([arm_l[0] - 8, arm_l[1] - 8, arm_l[0] + 8, arm_l[1] + 8], fill=PAL_STEEL[3], outline=C_INK, width=2)
    draw.rectangle([arm_r[0] - 8, arm_r[1] - 8, arm_r[0] + 8, arm_r[1] + 8], fill=PAL_STEEL[3], outline=C_INK, width=2)
    # Dangling broken chain links
    for i in range(4):
        draw_ink_circle(draw, [arm_l[0] - 4, arm_l[1] + 10 + i * 7, arm_l[0] + 4, arm_l[1] + 18 + i * 7], PAL_STEEL[0])
        draw_ink_circle(draw, [arm_r[0] - 4, arm_r[1] + 10 + i * 7, arm_r[0] + 4, arm_r[1] + 18 + i * 7], PAL_STEEL[0])

    # 5. Dual Brutal Falchion Cleavers
    draw_ink_poly(draw, [(arm_l[0] - 4, arm_l[1] - 6), (arm_l[0] - 20, arm_l[1] - 48), (arm_l[0] - 40, arm_l[1] - 36), (arm_l[0] - 8, arm_l[1] + 4)], PAL_STEEL[0])
    draw_ink_poly(draw, [(arm_r[0] + 4, arm_r[1] - 6), (arm_r[0] + 20, arm_r[1] - 48), (arm_r[0] + 40, arm_r[1] - 36), (arm_r[0] + 8, arm_r[1] + 4)], PAL_STEEL[0])

    if is_hurt:
        draw.line([(cx - 16, head_y - 4), (cx + 16, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_caiaphas_boss(frame_name, is_cast=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 86, 44
    if is_hurt: cx -= 6

    # Golden High Priest Miter with Golden Crown Plate
    draw_ink_poly(draw, [(cx - 22, head_y - 28), (cx + 22, head_y - 28), (cx + 16, head_y + 10), (cx - 16, head_y + 10)], (240, 245, 255, 255))
    draw_ink_poly(draw, [(cx - 18, head_y - 16), (cx + 18, head_y - 16), (cx + 14, head_y - 6), (cx - 14, head_y - 6)], PAL_GOLD[0])

    # Robe of the Ephod (Celestial Blue with Gold Fringe)
    draw_ink_poly(draw, [(cx - 22, torso_y - 10), (cx + 22, torso_y - 10), (cx + 28, ground_y - 6), (cx - 28, ground_y - 6)], PAL_COBALT[2])

    # Sacred Hoshen Breastplate with 12 Glowing Gemstones!
    bp_x, bp_y = cx - 18, torso_y - 2
    draw_ink_poly(draw, [(bp_x, bp_y), (bp_x + 36, bp_y), (bp_x + 36, bp_y + 28), (bp_x, bp_y + 28)], PAL_GOLD[1])
    gem_cols = [
        (255, 60, 60), (255, 220, 60), (60, 255, 120), (60, 200, 255),
        (220, 60, 255), (255, 140, 40), (40, 240, 200), (180, 255, 80),
        (255, 180, 220), (120, 60, 255), (255, 240, 180), (180, 140, 90)
    ]
    for idx, col in enumerate(gem_cols):
        gx = bp_x + 4 + (idx % 4) * 8
        gy = bp_y + 4 + (idx // 4) * 8
        draw.rectangle([gx, gy, gx + 5, gy + 5], fill=col)

    if is_cast:
        draw_capcom_muscle_limb(draw, (cx - 20, torso_y), (cx - 44, torso_y - 32), 8, 5, PAL_SKIN)
        draw_capcom_muscle_limb(draw, (cx + 20, torso_y), (cx + 44, torso_y - 32), 8, 5, PAL_SKIN)
        draw.ellipse([cx - 50, torso_y - 38, cx - 38, torso_y - 26], fill=(255, 255, 200, 255))
        draw.ellipse([cx + 38, torso_y - 38, cx + 50, torso_y - 26], fill=(255, 255, 200, 255))

    if is_hurt:
        draw.line([(cx - 14, head_y - 4), (cx + 14, head_y + 4)], fill=(240, 40, 40, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_judas_boss(frame_name, walk_phase=0, is_toss=False, is_dash=False, is_stab=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 82, 44
    if is_hurt: cx -= 6

    # Dishonored Olive/Ochre Cowl
    draw_ink_poly(draw, [(cx - 20, head_y - 20), (cx + 20, head_y - 20), (cx + 16, head_y + 20), (cx - 16, head_y + 20)], (65, 75, 45, 255))
    # Sneering shadowed face
    draw_ink_circle(draw, [cx - 12, head_y - 10, cx + 12, head_y + 12], PAL_SKIN[2])
    draw.point((cx - 4, head_y), fill=(20, 20, 20, 255))
    draw.point((cx + 4, head_y), fill=(20, 20, 20, 255))
    draw.arc([cx - 8, head_y + 6, cx + 8, head_y + 12], 180, 360, fill=(40, 20, 15, 255), width=2)

    # Dark Ochre Tunic & Mantle
    draw_ink_poly(draw, [(cx - 18, torso_y - 8), (cx + 18, torso_y - 8), (cx + 24, torso_y + 42), (cx - 24, torso_y + 42)], (85, 95, 55, 255))

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 40), (cx - 12 + int(stride), ground_y - 6), 7, 5, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 40), (cx + 12 - int(stride), ground_y - 6), 7, 5, PAL_SKIN)

    # Leather Pouch with 30 Pieces of Silver
    pouch_x = cx + 20
    draw_ink_circle(draw, [pouch_x - 10, torso_y + 20, pouch_x + 10, torso_y + 38], (120, 75, 40, 255))
    for dx, dy in [(-3, -5), (4, -7), (8, -2)]:
        draw_ink_circle(draw, [pouch_x + dx - 4, torso_y + 20 + dy - 4, pouch_x + dx + 4, torso_y + 20 + dy + 4], PAL_STEEL[0])

    if is_toss:
        draw_capcom_muscle_limb(draw, (cx - 16, torso_y), (cx - 42, torso_y - 10), 7, 5, PAL_SKIN)
        for i in range(4):
            sx = cx - 52 - i * 12
            sy = torso_y - 14 + i * 5
            draw_ink_circle(draw, [sx - 5, sy - 5, sx + 5, sy + 5], PAL_STEEL[0])

    if is_stab:
        draw_capcom_muscle_limb(draw, (cx - 16, torso_y), (cx - 52, torso_y + 6), 8, 6, PAL_SKIN)
        draw.line([(cx - 52, torso_y + 6), (cx - 82, torso_y + 6)], fill=PAL_STEEL[0], width=5)

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 40, 40, 255), width=3)

    save_sprite(img, frame_name)

create_capcom_money_changer("spr_money_changer_walk_0", walk_phase=0)
create_capcom_money_changer("spr_money_changer_walk_1", walk_phase=1)
create_capcom_money_changer("spr_money_changer_attack", is_attack=True)
create_capcom_money_changer("spr_money_changer_hurt", is_hurt=True)

create_capcom_levite_enforcer("spr_levite_enforcer_walk_0", walk_phase=0)
create_capcom_levite_enforcer("spr_levite_enforcer_walk_1", walk_phase=1)
create_capcom_levite_enforcer("spr_levite_enforcer_attack", is_attack=True)
create_capcom_levite_enforcer("spr_levite_enforcer_hurt", is_hurt=True)

create_capcom_barabbas_boss("spr_barabbas_idle")
create_capcom_barabbas_boss("spr_barabbas_walk_0", walk_phase=0)
create_capcom_barabbas_boss("spr_barabbas_walk_1", walk_phase=1)
create_capcom_barabbas_boss("spr_barabbas_slash", is_slash=True)
create_capcom_barabbas_boss("spr_barabbas_slam", is_slam=True)
create_capcom_barabbas_boss("spr_barabbas_hurt", is_hurt=True)

create_capcom_caiaphas_boss("spr_caiaphas_stand")
create_capcom_caiaphas_boss("spr_caiaphas_cast", is_cast=True)
create_capcom_caiaphas_boss("spr_caiaphas_hurt", is_hurt=True)

create_capcom_judas_boss("spr_judas_idle")
create_capcom_judas_boss("spr_judas_walk_0", walk_phase=0)
create_capcom_judas_boss("spr_judas_walk_1", walk_phase=1)
create_capcom_judas_boss("spr_judas_toss_silver", is_toss=True)
create_capcom_judas_boss("spr_judas_dash", is_dash=True)
create_capcom_judas_boss("spr_judas_stab", is_stab=True)
create_capcom_judas_boss("spr_judas_hurt", is_hurt=True)


# =========================================================================
# PART 7: MISSION 4 (THE JUDGMENT OF PONTIUS PILATE)
# =========================================================================
def create_capcom_signifer(frame_name, walk_phase=0, is_attack=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 82, 44
    if is_hurt: cx -= 6

    # Wolf Pelt Cowl (Wolf Head Drape with Ears & Fangs Framing Face)
    draw_ink_poly(draw, [(cx - 22, head_y - 28), (cx + 22, head_y - 28), (cx + 18, head_y + 14), (cx - 18, head_y + 14)], (115, 105, 95, 255))
    # Wolf ears
    draw_ink_poly(draw, [(cx - 20, head_y - 28), (cx - 12, head_y - 42), (cx - 6, head_y - 28)], (135, 125, 115, 255))
    draw_ink_poly(draw, [(cx + 6, head_y - 28), (cx + 12, head_y - 42), (cx + 20, head_y - 28)], (135, 125, 115, 255))
    # Face slit
    draw.rectangle([cx - 10, head_y - 4, cx + 10, head_y + 10], fill=PAL_SKIN[2])

    # Lorica Squamata (Scale Armor)
    draw_ink_poly(draw, [(cx - 20, torso_y - 8), (cx + 20, torso_y - 8), (cx + 18, torso_y + 36), (cx - 18, torso_y + 36)], PAL_BRONZE[1])

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 36), (cx - 10 + int(stride), ground_y - 6), 7, 5, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 36), (cx + 10 - int(stride), ground_y - 6), 7, 5, PAL_SKIN)

    # Towering Signum Standard (Golden Hand & Phalerae Medallions)
    pole_x = cx + 32
    draw.line([(pole_x, 10), (pole_x, ground_y)], fill=(130, 85, 40, 255), width=6)
    # Golden Hand (Manus) at apex
    draw_ink_circle(draw, [pole_x - 9, 8, pole_x + 9, 28], PAL_GOLD[0])
    # 4 Silver Phalerae Disks along shaft
    for dy in range(36, 92, 16):
        draw_ink_circle(draw, [pole_x - 11, dy, pole_x + 11, dy + 12], PAL_STEEL[0])

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_praetorian(frame_name, walk_phase=0, is_bash=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 86, 44
    if is_hurt: cx -= 6

    # Imperial Black & Gold Attic Helmet with Regal Purple Plume
    draw_ink_circle(draw, [cx - 24, head_y - 36, cx + 24, head_y - 16], (125, 20, 120, 255))
    draw_ink_circle(draw, [cx - 18, head_y - 18, cx + 18, head_y + 12], (35, 32, 42, 255))

    # Black Polished Lorica Segmentata with Gilded Joints
    draw_ink_poly(draw, [(cx - 22, torso_y - 10), (cx + 22, torso_y - 10), (cx + 20, torso_y + 36), (cx - 20, torso_y + 36)], (32, 30, 38, 255))
    for y in range(torso_y - 4, torso_y + 32, 8):
        draw.line([(cx - 18, y), (cx + 18, y)], fill=PAL_GOLD[1], width=3)

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 12, torso_y + 36), (cx - 12 + int(stride), ground_y - 6), 8, 6, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 12, torso_y + 36), (cx + 12 - int(stride), ground_y - 6), 8, 6, PAL_BRONZE)

    # Heavy Oval Praetorian Shield with Golden Imperial Scorpion
    sh_x = cx + 32 if not is_bash else cx - 32
    draw_ink_circle(draw, [sh_x - 16, torso_y - 18, sh_x + 16, torso_y + 46], (28, 25, 36, 255))
    # Golden Scorpion
    draw_ink_circle(draw, [sh_x - 6, torso_y + 6, sh_x + 6, torso_y + 18], PAL_GOLD[0])
    draw.arc([sh_x - 10, torso_y + 4, sh_x + 10, torso_y + 24], 0, 180, fill=PAL_GOLD[0], width=3)

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_lictor(frame_name, walk_phase=0, is_swing=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 82, 42
    if is_hurt: cx -= 6

    # Roman Toga Draped over Scarlet Tunic
    draw_ink_circle(draw, [cx - 14, head_y - 14, cx + 14, head_y + 14], PAL_SKIN[2])
    draw_ink_poly(draw, [(cx - 20, torso_y - 8), (cx + 20, torso_y - 8), (cx + 24, ground_y - 6), (cx - 24, ground_y - 6)], PAL_WRAP[1])

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 10, torso_y + 38), (cx - 10 + int(stride), ground_y - 6), 7, 5, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 10, torso_y + 38), (cx + 10 - int(stride), ground_y - 6), 7, 5, PAL_SKIN)

    # Roman Fasces Bundle (Birch Rods bound with Scarlet Thongs, Enclosing Bronze Axe)
    fasces_x = cx + 32 if not is_swing else cx - 44
    draw.line([(fasces_x, 16), (fasces_x, ground_y)], fill=(160, 115, 65, 255), width=10)
    for y in range(26, ground_y - 8, 16):
        draw.line([(fasces_x - 6, y), (fasces_x + 6, y)], fill=PAL_CRIMSON[1], width=3)
    # Executioner Bronze Axe Blade
    draw_ink_poly(draw, [(fasces_x + 5, 24), (fasces_x + 24, 14), (fasces_x + 28, 40), (fasces_x + 5, 34)], PAL_BRONZE[0])

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_prefect(frame_name, walk_phase=0, is_slash=False, is_hurt=False):
    W, H = 160, 192
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 86, 44
    if is_hurt: cx -= 6

    # Prefect Equestrian Crested Galea
    draw_ink_circle(draw, [cx - 26, head_y - 34, cx + 26, head_y - 14], PAL_CRIMSON[1])
    draw_ink_circle(draw, [cx - 18, head_y - 18, cx + 18, head_y + 14], PAL_GOLD[1])

    # Gilded Musculata Cuirass with Silver Gorgon Head
    draw_ink_poly(draw, [(cx - 26, torso_y - 14), (cx + 26, torso_y - 14), (cx + 22, torso_y + 38), (cx - 22, torso_y + 38)], PAL_GOLD[2])
    draw_ink_circle(draw, [cx - 10, torso_y, cx + 10, torso_y + 18], PAL_STEEL[0]) # Silver Medallion

    # Regal Purple Cape
    draw_ink_poly(draw, [(cx - 24, torso_y - 10), (cx - 56, torso_y + 68), (cx - 20, ground_y - 14), (cx + 24, torso_y - 10)], (115, 20, 110, 255))

    stride = math.sin(walk_phase * math.pi / 2.0) * 14.0
    draw_capcom_muscle_limb(draw, (cx - 12, torso_y + 38), (cx - 14 + int(stride), ground_y - 6), 10, 8, PAL_BRONZE)
    draw_capcom_muscle_limb(draw, (cx + 12, torso_y + 38), (cx + 14 - int(stride), ground_y - 6), 10, 8, PAL_BRONZE)

    # Spatha Heavy Sword
    draw.line([(cx + 24, torso_y), (cx + 56, torso_y - 28)], fill=PAL_STEEL[0], width=5)

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_advocate(frame_name, is_cast=False, is_shout=False, is_hurt=False):
    W, H = 144, 180
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 80, 42
    if is_hurt: cx -= 6

    # Patrician Toga with Gold Fibula Brooch
    draw_ink_circle(draw, [cx - 16, head_y - 16, cx + 16, head_y + 16], PAL_SKIN[2])
    draw_ink_poly(draw, [(cx - 20, torso_y - 10), (cx + 20, torso_y - 10), (cx + 26, ground_y - 6), (cx - 26, ground_y - 6)], (235, 230, 220, 255))
    draw_ink_circle(draw, [cx - 14, torso_y - 8, cx - 4, torso_y + 2], PAL_GOLD[0])

    # Holding Indictment Papyrus Scroll
    scroll_x = cx + 28
    draw.rectangle([scroll_x - 8, torso_y, scroll_x + 8, torso_y + 30], fill=(245, 235, 195, 255), outline=C_INK, width=2)

    if is_hurt:
        draw.line([(cx - 12, head_y - 4), (cx + 12, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

def create_capcom_pilate(frame_name, walk_phase=0, is_basin=False, is_gavel=False, is_hurt=False):
    # Final Boss: Pontius Pilate (176x208 Capcom Scale)
    W, H = 176, 208
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx, ground_y = W // 2, H - 10
    torso_y, head_y = 92, 48
    if is_hurt: cx -= 8

    # Golden Laurel Wreath
    draw.arc([cx - 22, head_y - 22, cx + 22, head_y + 10], 160, 380, fill=PAL_GOLD[1], width=5)
    for lx in range(-18, 19, 7):
        draw_ink_poly(draw, [(cx + lx, head_y - 20), (cx + lx + 5, head_y - 26), (cx + lx + 3, head_y - 18)], PAL_GOLD[0], outline_width=1)

    # Patrician Aristocratic Head with Silvered Temples
    draw_ink_circle(draw, [cx - 16, head_y - 16, cx + 16, head_y + 16], PAL_SKIN[1])
    draw.line([(cx - 18, head_y - 12), (cx - 14, head_y + 10)], fill=(185, 190, 200, 255), width=4)
    draw.line([(cx + 18, head_y - 12), (cx + 14, head_y + 10)], fill=(185, 190, 200, 255), width=4)

    # Roman Toga Praetexta (Broad Tyrian Purple Border Swag)
    draw_ink_poly(draw, [(cx - 28, torso_y - 12), (cx + 28, torso_y - 12), (cx + 32, ground_y - 6), (cx - 32, ground_y - 6)], PAL_WRAP[1])
    # Tyrian Purple Border Swag
    draw_ink_poly(draw, [(cx - 26, torso_y - 10), (cx - 12, torso_y - 10), (cx + 30, ground_y - 8), (cx + 16, ground_y - 8)], (115, 20, 110, 255))
    draw.line([(cx - 26, torso_y - 10), (cx + 16, ground_y - 8)], fill=PAL_GOLD[1], width=3)

    stride = math.sin(walk_phase * math.pi / 2.0) * 16.0
    draw_capcom_muscle_limb(draw, (cx - 12, ground_y - 18), (cx - 14 + int(stride), ground_y - 4), 8, 6, PAL_SKIN)
    draw_capcom_muscle_limb(draw, (cx + 12, ground_y - 18), (cx + 14 - int(stride), ground_y - 4), 8, 6, PAL_SKIN)

    # Ceremonial Bronze Water Basin
    if is_basin:
        bx = cx - 44
        by = torso_y + 24
        draw_ink_poly(draw, [(bx - 22, by), (bx + 22, by), (bx + 14, by + 32), (bx - 14, by + 32)], PAL_BRONZE[1])
        draw_ink_circle(draw, [bx - 18, by - 8, bx + 18, by + 8], (120, 210, 255, 220))
        # Hands washing in water
        draw_capcom_muscle_limb(draw, (cx - 16, torso_y), (bx - 2, by), 7, 5, PAL_SKIN)
        draw_capcom_muscle_limb(draw, (cx + 16, torso_y), (bx + 6, by), 7, 5, PAL_SKIN)

    if is_gavel:
        # Golden Stylus Blade Strike
        draw_capcom_muscle_limb(draw, (cx - 20, torso_y), (cx - 56, torso_y - 14), 8, 6, PAL_SKIN)
        draw.line([(cx - 56, torso_y - 14), (cx - 92, torso_y - 20)], fill=PAL_GOLD[0], width=6)

    if is_hurt:
        draw.line([(cx - 16, head_y - 4), (cx + 16, head_y + 4)], fill=(240, 50, 50, 255), width=3)

    save_sprite(img, frame_name)

create_capcom_signifer("spr_signifer_walk_0", walk_phase=0)
create_capcom_signifer("spr_signifer_walk_1", walk_phase=1)
create_capcom_signifer("spr_signifer_attack", is_attack=True)
create_capcom_signifer("spr_signifer_hurt", is_hurt=True)

create_capcom_praetorian("spr_praetorian_walk_0", walk_phase=0)
create_capcom_praetorian("spr_praetorian_walk_1", walk_phase=1)
create_capcom_praetorian("spr_praetorian_shield_bash", is_bash=True)
create_capcom_praetorian("spr_praetorian_hurt", is_hurt=True)

create_capcom_lictor("spr_lictor_walk_0", walk_phase=0)
create_capcom_lictor("spr_lictor_walk_1", walk_phase=1)
create_capcom_lictor("spr_lictor_fasces_swing", is_swing=True)
create_capcom_lictor("spr_lictor_hurt", is_hurt=True)

create_capcom_prefect("spr_prefect_idle")
create_capcom_prefect("spr_prefect_walk_0", walk_phase=0)
create_capcom_prefect("spr_prefect_walk_1", walk_phase=1)
create_capcom_prefect("spr_prefect_slash", is_slash=True)
create_capcom_prefect("spr_prefect_hurt", is_hurt=True)

create_capcom_advocate("spr_advocate_idle")
create_capcom_advocate("spr_advocate_cast", is_cast=True)
create_capcom_advocate("spr_advocate_shout", is_shout=True)
create_capcom_advocate("spr_advocate_hurt", is_hurt=True)

create_capcom_pilate("spr_pilate_idle")
create_capcom_pilate("spr_pilate_walk_0", walk_phase=0)
create_capcom_pilate("spr_pilate_walk_1", walk_phase=1)
create_capcom_pilate("spr_pilate_basin", is_basin=True)
create_capcom_pilate("spr_pilate_gavel_strike", is_gavel=True)
create_capcom_pilate("spr_pilate_hurt", is_hurt=True)

# =========================================================================
# PART 8: RELICS, MAGIC FX & PROJECTILES
# =========================================================================
def create_capcom_relics_and_fx():
    # 1. Dark Orb
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r, col in [(28, (120, 20, 180, 70)), (20, (170, 40, 230, 150)), (12, (220, 120, 255, 220)), (5, (255, 255, 255, 255))]:
        d.ellipse([32 - r, 32 - r, 32 + r, 32 + r], fill=col)
    save_sprite(img, "spr_dark_orb")

    # 2. Dark Spike
    img = Image.new("RGBA", (64, 96), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(32, 4), (58, 92), (6, 92)], (45, 15, 65, 255))
    d.polygon([(32, 16), (46, 88), (18, 88)], fill=(160, 40, 220, 255))
    save_sprite(img, "spr_dark_spike")

    # 3. Blood Shockwave
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([4, 4, 60, 60], outline=PAL_CRIMSON[1], width=5)
    d.ellipse([14, 14, 50, 50], outline=PAL_CRIMSON[0], width=3)
    save_sprite(img, "spr_blood_shockwave")

    # 4. Caiaphas Lightning
    img = Image.new("RGBA", (64, 384), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    points = [(32, 0)]
    for y in range(20, 380, 24):
        offset = 14 if (y // 24) % 2 == 0 else -14
        points.append((32 + offset, y))
    points.append((32, 384))
    d.line(points, fill=(180, 220, 255, 255), width=10)
    d.line(points, fill=(255, 255, 255, 255), width=4)
    save_sprite(img, "spr_caiaphas_lightning")

    # 5. Shout Wave
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r in range(12, 30, 8):
        d.arc([32 - r, 32 - r, 32 + r, 32 + r], -60, 60, fill=PAL_GOLD[0], width=4)
    save_sprite(img, "spr_shout_wave")

    # 6. Water Droplet
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(16, 4), (28, 24), (4, 24)], (120, 210, 255, 240))
    draw_ink_circle(d, [4, 14, 28, 28], (120, 210, 255, 240))
    save_sprite(img, "spr_water_droplet")

    # 7. Golden Stylus Blade
    img = Image.new("RGBA", (56, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(6, 16), (46, 8), (54, 16), (46, 24)], PAL_GOLD[0])
    save_sprite(img, "spr_golden_stylus_blade")

    # 8. Silver Pitcher
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(22, 14), (42, 14), (48, 54), (16, 54)], PAL_STEEL[1])
    d.arc([36, 18, 56, 46], 270, 90, fill=PAL_STEEL[0], width=4)
    save_sprite(img, "spr_silver_pitcher")

    # 9. Olive Wreath
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([10, 10, 54, 54], outline=(45, 125, 45, 255), width=7)
    for ang in range(0, 360, 30):
        rad = math.radians(ang)
        ox = int(32 + math.cos(rad) * 22)
        oy = int(32 + math.sin(rad) * 22)
        draw_ink_circle(d, [ox - 4, oy - 4, ox + 4, oy + 4], PAL_GOLD[0])
    save_sprite(img, "spr_olive_wreath")

    # 10. Sacred Chalice
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(18, 12), (46, 12), (40, 34), (24, 34)], PAL_GOLD[1])
    d.line([(32, 34), (32, 50)], fill=PAL_GOLD[0], width=5)
    draw_ink_poly(d, [(18, 54), (46, 54), (32, 50)], PAL_GOLD[2])
    save_sprite(img, "spr_sacred_chalice")

    # 11. Bitter Herbs
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.arc([16, 16, 48, 52], 180, 360, fill=(60, 160, 50, 255), width=6)
    draw_ink_circle(d, [18, 14, 32, 28], (90, 190, 70, 255))
    draw_ink_circle(d, [32, 14, 46, 28], (90, 190, 70, 255))
    save_sprite(img, "spr_bitter_herbs")

    # 12. Passover Matzot
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_circle(d, [10, 14, 54, 50], (225, 195, 140, 255))
    for x in range(18, 48, 8):
        for y in range(22, 44, 7):
            d.point((x, y), fill=(100, 65, 30, 255))
    save_sprite(img, "spr_passover_matzot")

    # 13. Holy Grail
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r in range(28, 0, -3):
        d.ellipse([32 - r, 32 - r, 32 + r, 32 + r], fill=(255, 240, 160, 18))
    draw_ink_poly(d, [(18, 12), (46, 12), (38, 36), (26, 36)], PAL_GOLD[0])
    d.line([(32, 36), (32, 50)], fill=PAL_GOLD[1], width=6)
    draw_ink_circle(d, [20, 48, 44, 56], PAL_GOLD[0])
    d.ellipse([22, 14, 42, 22], fill=PAL_CRIMSON[1])
    save_sprite(img, "spr_holy_grail")

    # 14. Roman Scroll
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(14, 18), (50, 18), (50, 46), (14, 46)], (245, 235, 195, 255))
    draw_ink_circle(d, [26, 26, 38, 38], PAL_CRIMSON[1])
    save_sprite(img, "spr_roman_scroll")

    # 15. Caesar Seal
    img = Image.new("RGBA", (56, 56), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_circle(d, [6, 6, 50, 50], PAL_GOLD[1])
    draw_ink_circle(d, [14, 14, 42, 42], PAL_GOLD[0])
    draw_ink_poly(d, [(28, 18), (34, 26), (30, 36), (24, 34)], PAL_GOLD[3])
    save_sprite(img, "spr_caesar_seal")

    # 16. Pilate Ewer
    img = Image.new("RGBA", (64, 72), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(20, 16), (44, 16), (50, 60), (14, 60)], PAL_BRONZE[1])
    d.arc([36, 20, 58, 52], 270, 90, fill=PAL_BRONZE[0], width=5)
    save_sprite(img, "spr_pilate_ewer")

    # 17. UI Heart Pip
    img = Image.new("RGBA", (40, 40), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(20, 36), (4, 18), (8, 6), (18, 8), (20, 12), (22, 8), (32, 6), (36, 18)], PAL_CRIMSON[1])
    d.ellipse([8, 8, 16, 16], fill=(255, 180, 190, 255))
    save_sprite(img, "spr_icon_heart")

    # 18. UI Wrath Icon
    img = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_circle(d, [4, 4, 44, 44], PAL_GOLD[1])
    draw_ink_circle(d, [10, 10, 38, 38], (60, 15, 20, 255))
    draw_ink_circle(d, [18, 16, 30, 28], PAL_GOLD[0])
    save_sprite(img, "spr_icon_wrath")

    # 19. Wicker Basket
    img = Image.new("RGBA", (88, 72), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(14, 20), (74, 20), (64, 66), (24, 66)], (175, 120, 60, 255))
    for x in range(20, 70, 8):
        d.line([(x, 22), (x, 64)], fill=(125, 80, 35, 255), width=3)
    for y in range(26, 62, 8):
        d.line([(18, y), (70, y)], fill=(225, 175, 100, 255), width=3)
    save_sprite(img, "spr_basket_loaves_fishes")

    # 20. Silver Coin Projectile
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_circle(d, [3, 3, 29, 29], PAL_STEEL[1])
    d.ellipse([6, 6, 26, 26], fill=PAL_STEEL[0])
    d.line([(16, 8), (16, 24)], fill=PAL_STEEL[3], width=3)
    save_sprite(img, "spr_silver_coin_projectile")

    # 21. Bread Loaf
    img = Image.new("RGBA", (40, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_circle(d, [4, 6, 36, 26], (215, 155, 80, 255))
    d.arc([8, 8, 32, 20], 180, 360, fill=(255, 210, 140, 255), width=4)
    save_sprite(img, "spr_bread_loaf")

    # 22. Fish Projectile
    img = Image.new("RGBA", (48, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_poly(d, [(6, 16), (16, 8), (34, 11), (42, 16), (34, 21), (16, 24)], (120, 185, 220, 255))
    draw_ink_poly(d, [(6, 16), (0, 8), (0, 24)], (80, 140, 180, 255))
    d.point((36, 13), fill=(20, 30, 40, 255))
    save_sprite(img, "spr_fish_projectile")

    # 23. Arrow
    img = Image.new("RGBA", (64, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.line([(4, 16), (56, 16)], fill=(140, 90, 45, 255), width=4)
    draw_ink_poly(d, [(52, 11), (62, 16), (52, 21)], PAL_STEEL[0])
    draw_ink_poly(d, [(4, 11), (14, 16), (4, 21)], PAL_CRIMSON[1])
    save_sprite(img, "spr_arrow")

    # 24. Censer Gold
    img = Image.new("RGBA", (72, 72), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    draw_ink_circle(d, [16, 32, 56, 64], PAL_GOLD[1])
    draw_ink_poly(d, [(36, 8), (22, 32), (50, 32)], PAL_GOLD[0])
    save_sprite(img, "spr_censer_gold")

    # 25. Whip Head
    img = Image.new("RGBA", (56, 56), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for dx, dy in [(12, 14), (28, 28), (44, 42)]:
        draw_ink_circle(d, [dx - 6, dy - 6, dx + 6, dy + 6], PAL_STEEL[0])
    d.line([(12, 14), (28, 28), (44, 42)], fill=PAL_STEEL[2], width=4)
    save_sprite(img, "spr_whip_head")

create_capcom_relics_and_fx()

print("ALL CAPCOM CPS-2 SPRITES GENERATED SUCCESSFULLY!")
